#include "Renderer.h"

#include "Manager.h"
#include "Renderpasses.h"
#include "PipelineCompiler.h"
#include "Vertex.h"

namespace Graphics::Renderer {

	/// <summary>
	/// Array of fences protecting the Per Frame Resources from double usage.
	/// </summary>
	static std::vector<vk::Fence> g_FrameResourceFences;
	/// <summary>
	/// Array of semaphores that will be signaled when a new swapchain image was acquired.
	/// </summary>
	static std::vector<vk::Semaphore> g_RenderStartSemaphores;
	/// <summary>
	/// Array of semaphores that will be signaled when a frame has finished rendering and is ready to be presented.
	/// </summary>
	static std::vector<vk::Semaphore> g_RenderFinishedSemaphores;

	/// <summary>
	/// The CommandPool used for rendering.
	/// </summary>
	static vk::CommandPool g_CommandPool;
	/// <summary>
	/// Array of CommandBuffers, one for each frame in flight.
	/// </summary>
	static std::vector<vk::CommandBuffer> g_CommandBuffers;

	/// <summary>
	/// Counter used to index the next set of per-frame resources.
	/// </summary>
	static uint32_t g_FrameCounter;

	/// <summary>
	/// Imageless framebuffer compatible with the 3D RenderPass.
	/// </summary>
	static vk::Framebuffer g_3DFramebuffer;

	/// <summary>
	/// Every Vulkan Pipeline needs a PipelineLayout that describes the layout of
	/// the DescriptorSets that will be passed to the shaders. Since our simple
	/// Pipeline does not use any descriptors, this Layout will be empty.
	/// </summary>
	static vk::PipelineLayout g_TestPipeLayout;
	/// <summary>
	/// A handle to our simple test GraphicsPipeline.
	/// A vk::Pipeline is roughly equivalent to a glProgram in OpenGL.
	/// </summary>
	static vk::Pipeline g_TestPipe;

	/// <summary>
	/// Information about our VertexBuffer.
	/// </summary>
	static Manager::BufferInfo g_VertexBuffer;

	void Initialize() {
		Renderpasses::Initialize();

		const auto& dev = Manager::GetDevice();

		g_FrameResourceFences.reserve(MAX_FRAMES_IN_FLIGHT);
		for(auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			// Fences must be signaled on creation to avoid a dead lock on the first call to RenderFrame().
			vk::FenceCreateInfo fInfo{ vk::FenceCreateFlagBits::eSignaled };
			g_FrameResourceFences.push_back(dev.createFence(fInfo));
		}

		g_RenderStartSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
		g_RenderFinishedSemaphores.reserve(MAX_FRAMES_IN_FLIGHT);
		for (auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
			vk::SemaphoreCreateInfo sInfo{};
			g_RenderStartSemaphores.push_back(dev.createSemaphore(sInfo));
			g_RenderFinishedSemaphores.push_back(dev.createSemaphore(sInfo));
		}

		vk::CommandPoolCreateInfo poolInfo{
			vk::CommandPoolCreateFlagBits::eResetCommandBuffer, Manager::GetGraphicsQueueFamily()
		};
		g_CommandPool = dev.createCommandPool(poolInfo);

		vk::CommandBufferAllocateInfo cbInfo{
				g_CommandPool, vk::CommandBufferLevel::ePrimary, (uint32_t)MAX_FRAMES_IN_FLIGHT
		};
		g_CommandBuffers = dev.allocateCommandBuffers(cbInfo);

		g_TestPipeLayout = Manager::GetDevice().createPipelineLayout({
			{}, {}, {}
		});
		g_TestPipe = PipelineCompiler::Compile("Assets/Shaders/triangle", g_TestPipeLayout, Renderpasses::Get3DPass(), 0);

		// Create a VertexBuffer to hold our three vertices.
		g_VertexBuffer = Manager::CreateBuffer(sizeof(Vertex) * 3, vk::BufferUsageFlagBits::eVertexBuffer, Manager::BufferType::Staging);
		// Map the buffer into application-visible memory, so we can copy data to the buffer.
		auto buffer = Manager::MapAllocation(g_VertexBuffer.allocation);

		// copy the data into the buffer.
		Vertex vertices[]{
			{ -1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f },
			{ 0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f },
			{ 1.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f },
		};
		memcpy(buffer, vertices, sizeof(vertices));

		// unmap the buffer as we don't need to access it anymore.
		Manager::UnmapAllocation(g_VertexBuffer.allocation);
	}

	void Terminate() {
		const auto& dev = Manager::GetDevice();

		Manager::DestroyBuffer(g_VertexBuffer);

		dev.destroyPipeline(g_TestPipe);
		dev.destroyPipelineLayout(g_TestPipeLayout);

		dev.destroyFramebuffer(g_3DFramebuffer);
		// destroying the CommandPool automatically destroys all allocated CommandBuffers.
		dev.destroyCommandPool(g_CommandPool);

		for (const auto& s : g_RenderFinishedSemaphores)
			dev.destroySemaphore(s);
		for (const auto& s : g_RenderStartSemaphores)
			dev.destroySemaphore(s);
		for (const auto& f : g_FrameResourceFences)
			dev.destroyFence(f);

		Renderpasses::Terminate();
	}

	/// <summary>
	/// Recreates the framebuffers. Must be called when a window changed size.
	/// </summary>
	/// <param name="size">New size of the framebuffers</param>
	static void RecreateFramebuffers(const vk::Extent2D& size) {
		// Destroy the old framebuffer.
		if (g_3DFramebuffer)
			Manager::GetDevice().destroyFramebuffer(g_3DFramebuffer);

		vk::FramebufferCreateInfo fbInfo{
			vk::FramebufferCreateFlagBits::eImageless, Renderpasses::Get3DPass(),
			1, nullptr,
			size.width, size.height, 1
		};
		// TODO: use correct format
		auto fmt = vk::Format::eB8G8R8A8Srgb;
		// for imageless framebuffers, we need to specify the format and usage flags of the ImageViews that will be used later.
		std::array atInfos{
			vk::FramebufferAttachmentImageInfo {
				{}, vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
				size.width, size.height, 1, fmt
			},
		};
		vk::FramebufferAttachmentsCreateInfo atInfo{
			atInfos
		};
		fbInfo.pNext = &atInfo;

		g_3DFramebuffer = Manager::GetDevice().createFramebuffer(fbInfo);
	}

	void RenderFrame(Window& wnd) {
		// If we haven't created a framebuffer yet, do that now. Should only happen on the first frame.
		if (!g_3DFramebuffer)
			RecreateFramebuffers(wnd.GetExtent());

		const auto& dev = Manager::GetDevice();

		/*
		 * Rendering a frame consists of the following steps:
		 * - Ensure that the resources for the next frame are not in use anymore (waitForFences).
		 * - Acquire the next swapchain image (acquireNextImageKHR).
		 * - Record a command buffer containing the necessary commands to render a frame to the acquired swapchain image.
		 * - Present the image (presentKHR).
		 */

		// waitForFences should always return Success, compiler will complain anyways if we don't use the result.
		auto ignore = dev.waitForFences(g_FrameResourceFences[g_FrameCounter], true, UINT64_MAX) == vk::Result::eSuccess;
		dev.resetFences(g_FrameResourceFences[g_FrameCounter]);

		uint32_t imageIndex;
		try {
			// g_RenderStartSemaphores[g_FrameCounter] will be signaled when the acquired image is ready to be rendered to.
			imageIndex = dev.acquireNextImageKHR(wnd.GetSwapchain(), UINT64_MAX, g_RenderStartSemaphores[g_FrameCounter], nullptr).value;
		} catch(const vk::OutOfDateKHRError&) {
			// When a window is resized, the swapchain might not be compatible with that window anymore, in which case the above error will be thrown.
			// We then have to create an entirely new swapchain with the new size in order to continue rendering.

			// Since we cannot destroy a swapchain that is currently being rendered to, we just wait for the device to become idle.
			Manager::GetDevice().waitIdle();
			wnd.ResizeSwapchain();
			// Since the swapchain size changed, we also need a new framebuffer. (Don't ask me why an imageless framebuffer needs to specify a size)
			RecreateFramebuffers(wnd.GetExtent());
			return;
		}

		auto& cmd = g_CommandBuffers[g_FrameCounter];

		vk::CommandBufferBeginInfo cmdInfo{
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit // This CommandBuffer will only be submitted once before it will be recorded again.
		};
		cmd.begin(cmdInfo);

		// Here we specify which color the color attachment should be cleared to.
		vk::ClearValue clearColor{ vk::ClearColorValue{std::array{0.2f, 0.2f, 0.2f, 1.0f}} };
		vk::RenderPassBeginInfo rpInfo{
			Renderpasses::Get3DPass(), g_3DFramebuffer, vk::Rect2D{{0, 0}, wnd.GetExtent()},
			clearColor
		};
		// Since we are using an imageless framebuffer, we need to pass a vk::RenderPassAttachmentBeginInfo, containing the actual ImageViews we want to render to.
		vk::RenderPassAttachmentBeginInfo atInfo{
			wnd.GetImageViews()[imageIndex]
		};
		rpInfo.pNext = &atInfo;
		cmd.beginRenderPass(rpInfo, vk::SubpassContents::eInline);

		// This is the equivalent to glUseProgram. Every draw command after this will use the given Pipeline.
		cmd.bindPipeline(vk::PipelineBindPoint::eGraphics, g_TestPipe);

		// Since we created our Pipeline with dynamic Viewport and Scissor sizes, we need to specify
		// those dimensions before we draw anything.
		auto extent = wnd.GetExtent();
		cmd.setViewport(0, vk::Viewport{
			0.0f, 0.0f, (float)extent.width, (float)extent.height, 0.0f, 1.0f
		});
		cmd.setScissor(0, vk::Rect2D {
			vk::Offset2D{0, 0},
			extent
		});

		// Since our shader expects a VertexBuffer containing data at binding 0, we need to tell Vulkan which buffer to use.
		cmd.bindVertexBuffers(0, g_VertexBuffer.buffer, { 0 });

		// Roughly equivalent to glDrawArraysInstanced.
		// The vertex data is located in our vertex buffer.
		cmd.draw(3, 1, 0, 0);

		cmd.endRenderPass();
		cmd.end();

		// The commands recorded above may start executing before the swapchain image is ready to be rendered to.
		// To prevent that, we specify that the given CommandBuffer should not execute anything in the ColorAttachmentOutput stage
		// before the semaphore is signaled. Any vertex processing etc. can then start executing before the swapchain image is ready,
		// only actual pixel output is delayed.
		vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vk::SubmitInfo submitInfo{
			g_RenderStartSemaphores[g_FrameCounter],
			waitStage,
			cmd,
			g_RenderFinishedSemaphores[g_FrameCounter],
		};
		Manager::GetGraphicsQueue().submit(submitInfo, g_FrameResourceFences[g_FrameCounter]);

		// We need to wait for rendering to be finished before we can present a swapchain image, as otherwise a
		// half-finished image might be presented. Thus, we specify g_RenderFinishedSemaphore[g_FrameCounter], which will be signalled by the
		// submit() call above, once all commands are finished.
		auto sc = wnd.GetSwapchain();
		vk::PresentInfoKHR presentInfo{
			g_RenderFinishedSemaphores[g_FrameCounter],
			sc,
			imageIndex
		};
		try {
			auto err = Manager::GetGraphicsQueue().presentKHR(presentInfo);
		} catch(const vk::OutOfDateKHRError&) {
			// See the try/catch block of acquireNextImageKHR for explanation.
			Manager::GetDevice().waitIdle();
			wnd.ResizeSwapchain();
			RecreateFramebuffers(wnd.GetExtent());
			return;
		}

		g_FrameCounter++;
		g_FrameCounter %= MAX_FRAMES_IN_FLIGHT;
	}

}

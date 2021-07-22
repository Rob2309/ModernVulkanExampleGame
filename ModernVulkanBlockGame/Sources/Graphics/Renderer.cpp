#include "Renderer.h"

#include "Manager.h"
#include "Renderpasses.h"

namespace Graphics::Renderer {

	static std::vector<vk::Fence> g_FrameResourceFences;
	static std::vector<vk::Semaphore> g_RenderStartSemaphores;
	static std::vector<vk::Semaphore> g_RenderFinishedSemaphores;

	static vk::CommandPool g_CommandPool;
	static std::vector<vk::CommandBuffer> g_CommandBuffers;

	static uint32_t g_FrameCounter;

	static vk::Framebuffer g_3DFramebuffer;

	void Initialize() {
		Renderpasses::Initialize();

		const auto& dev = Manager::GetDevice();

		g_FrameResourceFences.reserve(MAX_FRAMES_IN_FLIGHT);
		for(auto i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
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
	}

	void Terminate() {
		const auto& dev = Manager::GetDevice();

		dev.destroyFramebuffer(g_3DFramebuffer);
		dev.destroyCommandPool(g_CommandPool);

		for (const auto& s : g_RenderFinishedSemaphores)
			dev.destroySemaphore(s);
		for (const auto& s : g_RenderStartSemaphores)
			dev.destroySemaphore(s);
		for (const auto& f : g_FrameResourceFences)
			dev.destroyFence(f);

		Renderpasses::Terminate();
	}

	void RecreateFramebuffers(const vk::Extent2D& size) {
		if (g_3DFramebuffer)
			Manager::GetDevice().destroyFramebuffer(g_3DFramebuffer);

		vk::FramebufferCreateInfo fbInfo{
			vk::FramebufferCreateFlagBits::eImageless, Renderpasses::Get3DPass(),
			1, nullptr,
			size.width, size.height, 1
		};
		auto fmt = vk::Format::eB8G8R8A8Srgb;
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
		if (!g_3DFramebuffer)
			RecreateFramebuffers(wnd.GetExtent());

		const auto& dev = Manager::GetDevice();

		assert(dev.waitForFences(g_FrameResourceFences[g_FrameCounter], true, UINT64_MAX) == vk::Result::eSuccess);
		dev.resetFences(g_FrameResourceFences[g_FrameCounter]);

		uint32_t imageIndex;
		try {
			imageIndex = dev.acquireNextImageKHR(wnd.GetSwapchain(), UINT64_MAX, g_RenderStartSemaphores[g_FrameCounter], nullptr).value;
		} catch(const vk::OutOfDateKHRError&) {
			Manager::GetDevice().waitIdle();
			wnd.ResizeSwapchain();
			RecreateFramebuffers(wnd.GetExtent());
			return;
		}

		auto& cmd = g_CommandBuffers[g_FrameCounter];

		vk::CommandBufferBeginInfo cmdInfo{
			vk::CommandBufferUsageFlagBits::eOneTimeSubmit
		};
		cmd.begin(cmdInfo);

		vk::ClearValue clearColor{ vk::ClearColorValue{std::array{1.0f, 0.0f, 1.0f, 1.0f}} };
		vk::RenderPassBeginInfo rpInfo{
			Renderpasses::Get3DPass(), g_3DFramebuffer, vk::Rect2D{{0, 0}, wnd.GetExtent()},
			clearColor
		};
		vk::RenderPassAttachmentBeginInfo atInfo{
			wnd.GetImageViews()[imageIndex]
		};
		rpInfo.pNext = &atInfo;
		cmd.beginRenderPass(rpInfo, vk::SubpassContents::eInline);

		cmd.endRenderPass();
		cmd.end();

		vk::PipelineStageFlags waitStage = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vk::SubmitInfo submitInfo{
			g_RenderStartSemaphores[g_FrameCounter],
			waitStage,
			cmd,
			g_RenderFinishedSemaphores[g_FrameCounter],
		};
		Manager::GetGraphicsQueue().submit(submitInfo, g_FrameResourceFences[g_FrameCounter]);

		auto sc = wnd.GetSwapchain();
		vk::PresentInfoKHR presentInfo{
			g_RenderFinishedSemaphores[g_FrameCounter],
			sc,
			imageIndex
		};
		try {
			Manager::GetGraphicsQueue().presentKHR(presentInfo);
		} catch(const vk::OutOfDateKHRError&) {
			Manager::GetDevice().waitIdle();
			wnd.ResizeSwapchain();
			RecreateFramebuffers(wnd.GetExtent());
			return;
		}

		g_FrameCounter++;
		g_FrameCounter %= MAX_FRAMES_IN_FLIGHT;
	}

}

#include "Renderpasses.h"

#include "Manager.h"

namespace Graphics::Renderpasses {

	/// <summary>
	/// The RenderPass used for rendering 3D scenes.
	/// </summary>
	static vk::RenderPass g_3DPass;

	void Initialize() {
		// Here we describe all the framebuffer attachments that are expected by the RenderPass.
		/*
		 * Each AttachmentDescription2 describes a single attachment used by the RenderPass.
		 *
		 * The loadOp/storeOp and stencilLoadOp/stencilStoreOp pairs describe what happens to the data stored in the attachment at certain points in the pipeline.
		 * loadOp specifies what happens with data already stored in the attachment before any rendering begins. This will probably be the pixels of the previous frame.
		 * By using eClear, we discard the previous frame's pixels and set them to a color specified later in a beginRenderPass() command.
		 * storeOp specifies what happens to the pixels in the attachment after the entire RenderPass has finished executing.
		 * For a temporary attachment that is not needed after rendering, you could specify eDontCare, but since we need the pixels for presenting them to the screen, we
		 * specify eStore.
		 * The same applies to stencilLoadOp and stencilStoreOp.
		 *
		 * The initialLayout and finalLayout pair functions in a similar fashion to the above pairs.
		 * initialLayout specifies in what ImageLayout the given attachment will be in, before any rendering starts.
		 * finalLayout specifies what layout the Image should be in after any rendering has finished. The transition to this layout will
		 * automatically be handled.
		 * Since we don't care about the content of the image when we begin rendering, we can just pass eUndefined.
		 * After rendering finished, we want to present the image to the screen, so we use this opportunity to automatically transition
		 * it to the required layout (ePresentSrcKHR).
		 */
		std::array attachments{
			// Color Attachment (will be a swapchain image).
			vk::AttachmentDescription2 {
				{}, vk::Format::eB8G8R8A8Srgb,
				vk::SampleCountFlagBits::e1,
				vk::AttachmentLoadOp::eClear, vk::AttachmentStoreOp::eStore,
				vk::AttachmentLoadOp::eDontCare, vk::AttachmentStoreOp::eDontCare,
				vk::ImageLayout::eUndefined, vk::ImageLayout::ePresentSrcKHR
			},
		};
		std::array refs{
			vk::AttachmentReference2 {
				0, vk::ImageLayout::eColorAttachmentOptimal, vk::ImageAspectFlagBits::eColor
			},
		};
		/*
		 * Here we describe the SubPasses that the RenderPass will contain. A SubPass is a "step" in our rendering pipeline.
		 * E.g. one subpass could be used to render shadow maps, while another could be used to then render the actual scene using those shadow maps.
		 * Currently we only need one subpass, as we don't do anything fancy yet.
		 */
		std::array subpasses{
			vk::SubpassDescription2 {
				{}, vk::PipelineBindPoint::eGraphics, 0,
				{},
				refs, // here we specify which attachments are used in this subpass.
				{},
				nullptr,
				{},
			}
		};
		// Here we can describe what subpasses depend on each other. For the shadowmap example above, we would need to specify that the scene rendering subpass depends
		// on the results of the shadowmap subpass.
		std::array deps{
			/*
			 * We also need to specify a special dependency to ensure that the automatic layout transition from eUndefined to eColorAttachmentOptimal
			 * happens *after* presenting the previous frame has finished.
			 * Therefore, we need to wait for the previous eColorAttachmentOutput stage to be finished before we start out eColorAttachmentOutput stage.
			 * Specifically, we cannot write to the color attachment before presentation has finished, thus we specify eColorAttachmentWrite as destination access mask.
			 */
			vk::SubpassDependency2 {
				VK_SUBPASS_EXTERNAL, 0,
				vk::PipelineStageFlagBits::eColorAttachmentOutput, vk::PipelineStageFlagBits::eColorAttachmentOutput,
				{}, vk::AccessFlagBits::eColorAttachmentWrite,
				vk::DependencyFlagBits::eByRegion,
				0
			},
		};
		vk::RenderPassCreateInfo2 passInfo{
			{},
			attachments,
			subpasses,
			deps,
			{},
		};
		g_3DPass = Manager::GetDevice().createRenderPass2(passInfo);
	}

	void Terminate() {
		Manager::GetDevice().destroyRenderPass(g_3DPass);
	}

	vk::RenderPass Get3DPass() {
		return g_3DPass;
	}

}

#include "Renderpasses.h"

#include "Manager.h"

namespace Graphics::Renderpasses {

	static vk::RenderPass g_3DPass;

	void Initialize() {
		std::array attachments{
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
		std::array subpasses{
			vk::SubpassDescription2 {
				{}, vk::PipelineBindPoint::eGraphics, 0,
				{},
				refs,
				{},
				nullptr,
				{},
			}
		};
		std::array deps{
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

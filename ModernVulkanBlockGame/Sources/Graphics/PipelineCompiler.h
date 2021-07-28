#pragma once

#include <vulkan/vulkan.hpp>

namespace Graphics::PipelineCompiler {

    vk::Pipeline Compile(const std::string& shaderName, vk::PipelineLayout layout, vk::RenderPass renderpass, uint32_t subpass);

}

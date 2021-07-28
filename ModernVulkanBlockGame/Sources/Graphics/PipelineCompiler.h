#pragma once

#include <vulkan/vulkan.hpp>

namespace Graphics::PipelineCompiler {

    /// <summary>
    /// Compiles a very basic vk::GraphicsPipeline. Mainly used to improve code readability, since
    /// creating a vk::Pipeline involves ~60 lines of code.
    /// </summary>
    /// <param name="shaderName">Path to a shader without the .hlsl extension</param>
    /// <param name="layout">A compatible vk::PipelineLayout</param>
    /// <param name="renderpass">A compatible vk::RenderPass</param>
    /// <param name="subpass">Which subpass the Pipeline will be used in</param>
    /// <returns>The compiled vk::Pipeline</returns>
    vk::Pipeline Compile(const std::string& shaderName, vk::PipelineLayout layout, vk::RenderPass renderpass, uint32_t subpass);

}

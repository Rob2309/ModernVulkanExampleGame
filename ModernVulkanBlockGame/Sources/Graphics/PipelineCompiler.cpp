#include "PipelineCompiler.h"

#include "Manager.h"

#include <filesystem>
#include <fstream>

namespace Graphics::PipelineCompiler {

    static std::vector<char> ReadFile(const std::string& path) {
        auto size = std::filesystem::file_size(path);

        std::vector<char> buffer;
        buffer.resize(size);

        std::ifstream in{path, std::ifstream::in | std::ifstream::binary};
        in.read(buffer.data(), size);
        in.close();

        return buffer;
    }

    static vk::ShaderModule CreateModule(const std::string& path) {
        auto code = ReadFile(path);

        return Manager::GetDevice().createShaderModule({
            {}, code.size(), reinterpret_cast<uint32_t*>(code.data())
        });
    }

    vk::Pipeline Compile(const std::string& shaderName, vk::PipelineLayout layout, vk::RenderPass renderpass, uint32_t subpass) {
        auto vShaderMod = CreateModule(shaderName + ".vert.spv");
        auto fShaderMod = CreateModule(shaderName + ".frag.spv");

        std::array stages {
            vk::PipelineShaderStageCreateInfo{ {}, vk::ShaderStageFlagBits::eVertex, vShaderMod, "vert" },
            vk::PipelineShaderStageCreateInfo{ {}, vk::ShaderStageFlagBits::eFragment, fShaderMod, "frag" },
        };

        vk::PipelineVertexInputStateCreateInfo vertexInput {
            {}, {}, {}
        };

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly {
            {}, vk::PrimitiveTopology::eTriangleList, false
        };

        vk::PipelineViewportStateCreateInfo viewport {
            {}, 1, nullptr, 1, nullptr
        };

        vk::PipelineRasterizationStateCreateInfo rasterization {
            {}, false, false,
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise,
            false, 0, 0, 0,
            1.0f // Probably one of the most annoying things about the Vulkan spec
        };

        vk::PipelineMultisampleStateCreateInfo multisample {
            {}, vk::SampleCountFlagBits::e1,
            false
        };

        std::array blendAttachments {
            vk::PipelineColorBlendAttachmentState {
                false, {}, {}, {}, {}, {}, {},
                vk::ColorComponentFlagBits::eA | vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB 
            },
        };
        vk::PipelineColorBlendStateCreateInfo colorBlend {
            {}, false, {},
            blendAttachments
        };

        std::array dynamicStates {
            vk::DynamicState::eViewport,
            vk::DynamicState::eScissor,
        };
        vk::PipelineDynamicStateCreateInfo dynamic {
            {}, dynamicStates
        };

        vk::GraphicsPipelineCreateInfo pipeInfo {
            {},
            stages,
            &vertexInput,
            &inputAssembly,
            nullptr,
            &viewport,
            &rasterization,
            &multisample,
            nullptr,
            &colorBlend,
            &dynamic,
            layout, renderpass, subpass
        };
        auto res = Manager::GetDevice().createGraphicsPipeline(nullptr, pipeInfo).value;

        Manager::GetDevice().destroyShaderModule(vShaderMod);
        Manager::GetDevice().destroyShaderModule(fShaderMod);

        return res;
    }

}

#include "PipelineCompiler.h"

#include "Manager.h"
#include "Vertex.h"

#include <filesystem>
#include <fstream>

namespace Graphics::PipelineCompiler {

    /// <summary>
    /// Read all bytes in a file
    /// </summary>
    /// <param name="path">Path to the file</param>
    /// <returns>std::vector<char> containing all bytes in the file</returns>
    static std::vector<char> ReadFile(const std::string& path) {
        auto size = std::filesystem::file_size(path);

        std::vector<char> buffer;
        buffer.resize(size);

        std::ifstream in{path, std::ifstream::in | std::ifstream::binary};
        in.read(buffer.data(), size);
        in.close();

        return buffer;
    }

    /// <summary>
    /// Create a vk::ShaderModule from a give shader path
    /// </summary>
    /// <param name="path">Path to the SPIR-V file</param>
    /// <returns>vk::ShaderModule with the given SPIR-V code</returns>
    static vk::ShaderModule CreateModule(const std::string& path) {
        auto code = ReadFile(path);

        return Manager::GetDevice().createShaderModule({
            {}, code.size(), reinterpret_cast<uint32_t*>(code.data())
        });
    }

    vk::Pipeline Compile(const std::string& shaderName, vk::PipelineLayout layout, vk::RenderPass renderpass, uint32_t subpass) {
    	// load vertex and fragment shader modules
        auto vShaderMod = CreateModule(shaderName + ".vert.spv");
        auto fShaderMod = CreateModule(shaderName + ".frag.spv");

    	// This array specifies every programmable shader stage the pipeline will use.
    	// We could e.g. specify further shaders like a geometry shader. For now we only use Vertex and Fragment shader.
        std::array stages {
            vk::PipelineShaderStageCreateInfo{ {}, vk::ShaderStageFlagBits::eVertex, vShaderMod, "vert" }, // "vert" is the name of the shaders main function.
            vk::PipelineShaderStageCreateInfo{ {}, vk::ShaderStageFlagBits::eFragment, fShaderMod, "frag" },
        };

    	/*
    	 * Here we specify Vertex Attributes and Bindings that the Vertex Shader will receive.
    	 * Equivalent functionality is available in OpenGL via glVertexAttribPointer/glVertexAttribFormat/glVertexAttribBinding.
    	 * The Vertex shader will receive this data via
    	 *      layout (location=XXX) in vec3 ...; in GLSL
    	 *      or simple input variables in HLSL (see struct Vertex in triangle.hlsl).
    	 */
        std::array vertexBindings{
            vk::VertexInputBindingDescription { 0, sizeof(Vertex), vk::VertexInputRate::eVertex }, // Vertex buffer at binding zero contains vertices of size sizeof(Vertex).
        };
        std::array vertexAttribs{
            // Vertex Attribute 0 is 3 32-bit floats, starting at byte 0 (offsetof(Vertex, position)) of each Vertex.
            vk::VertexInputAttributeDescription { 0, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, position) },
            // Vertex Attribute 1 is 3 32-bit floats, starting at byte 12 (offsetof(Vertex, color)) of each Vertex.
            vk::VertexInputAttributeDescription { 1, 0, vk::Format::eR32G32B32Sfloat, offsetof(Vertex, color) },
        };
        vk::PipelineVertexInputStateCreateInfo vertexInput {
            {},
            vertexBindings,
            vertexAttribs,
        };

    	/*
    	 * This struct describes how the vertices should be put together into renderable primitives.
    	 * Since we want to render each set of three vertices as a triangle, we choose TriangleList.
    	 */
        vk::PipelineInputAssemblyStateCreateInfo inputAssembly {
            {}, vk::PrimitiveTopology::eTriangleList, false
        };

    	/*
    	 * Normally we would need to specify the size of the viewport and scissor of the pipeline.
    	 * This means that we would need to recreate every pipeline as soon as the swapchain (and thus our viewport)
    	 * changes size.
    	 * Since we don't want to recreate pipelines often, we will use a dynamic viewport and scissor state (see below).
    	 * This allows us to not specify any dimensions here. We can specify the dimensions in a command buffer later.
    	 * viewportCount and scissorCount must still be valid numbers, but pViewports and pScissors will be ignored.
    	 */
        vk::PipelineViewportStateCreateInfo viewport {
            {}, 1, nullptr, 1, nullptr
        };

    	/*
    	 * This struct specifies how each primitive (triangles in our case) will be rasterized into
    	 * fragments/pixels.
    	 * vk::PolygonMode::eFill does exactly what it sounds like, each triangle will be filled in with pixels, rather
    	 * than outlined with vk::PolygonMode::eLine for example.
    	 * We then specify that we don't want to render the backside of a triangle, since we will never see it with normal meshes.
    	 * The backside of a triangle will be the side where the three vertices are be counter clockwise.
    	 *
    	 * Even though we use Fill mode, we still must specify a lineWidth of 1.0, which is a little annoyance in the spec.
    	 */
        vk::PipelineRasterizationStateCreateInfo rasterization {
            {}, false, false,
            vk::PolygonMode::eFill,
            vk::CullModeFlagBits::eBack, vk::FrontFace::eClockwise,
            false, 0, 0, 0,
            1.0f // Probably one of the most annoying things about the Vulkan spec
        };

    	/*
    	 * In this struct, we would specify information about multisampling, if we used it.
    	 * For now, we just need to specify vk::SampleCountFlagBits::e1 to indicate that we don't use any multisampling.
    	 */
        vk::PipelineMultisampleStateCreateInfo multisample {
            {}, vk::SampleCountFlagBits::e1,
            false
        };

    	/*
    	 * In this struct, we specify, for each color attachment in our RenderPass, how the output of the fragment shader will be written
    	 * into the corresponding attachment image.
    	 * We disable blending, meaning the output of the fragment shader will simply overwrite any value that was present in
    	 * the color attachment before.
    	 * We also specify that the fragment shader is allowed to write every channel of the attachment via vk::ColorComponentFlagBits.
    	 * A simple "nightvision" filter could e.g. be implemented by only specifying vk::ColorComponentFlagsBits::eG, which would prevent the fragment shader
    	 * from writing any color except tones of green.
    	 */
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

    	/*
    	 * Here we specify that we want to use a dynamically sized viewport and scissor, see above.
    	 */
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
            nullptr, // tesselationState is only needed when we add a tesselation shader
            &viewport,
            &rasterization,
            &multisample,
            nullptr, // depthStencilState is only needed when we use a depth/stencil attachment in the RenderPass.
            &colorBlend,
            &dynamic,
            layout, renderpass, subpass
        };
    	/*
    	 * This call will compile the SPIR-V code into a fully functional and usable vk::Pipeline.
    	 * One of the huge advantages of Vulkan over e.g. OpenGL is that we can accurately control when
    	 * this compilation takes place.
    	 */
        auto res = Manager::GetDevice().createGraphicsPipeline(nullptr, pipeInfo).value;

    	// After the pipeline is created, the shader modules are not needed anymore.
        Manager::GetDevice().destroyShaderModule(vShaderMod);
        Manager::GetDevice().destroyShaderModule(fShaderMod);

        return res;
    }

}

#pragma once

#include <vulkan/vulkan.hpp>

namespace Graphics::Renderpasses {

	/// <summary>
	/// Initializes all RenderPasses required by the application.
	/// </summary>
	///	<remarks>Must be called after Manager::Initialize(), automatically called by Renderer::Initialize()</remarks>
	void Initialize();

	/// <summary>
	/// Deinitializes all RenderPasses required by the application.
	/// </summary>
	///	<remarks>Must be called before Manager::Terminate(), automatically called by Renderer::Terminate()</remarks>
	void Terminate();

	/// <returns>The RenderPass used for rendering 3D scenes.</returns>
	vk::RenderPass Get3DPass();

}


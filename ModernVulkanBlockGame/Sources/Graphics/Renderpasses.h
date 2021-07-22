#pragma once

#include <vulkan/vulkan.hpp>

namespace Graphics::Renderpasses {

	void Initialize();
	void Terminate();

	vk::RenderPass Get3DPass();

}


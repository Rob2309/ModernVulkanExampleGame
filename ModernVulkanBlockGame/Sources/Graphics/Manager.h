#pragma once

#include <vulkan/vulkan.hpp>

namespace Graphics::Manager {

	/// <summary>
	/// Initializes the basic Graphics system.
	/// </summary>
	///	<remarks>Must be called before creating a window.</remarks>
	bool Initialize();

	/// <summary>
	/// Deinitializes the basic Graphics system.
	/// </summary>
	///	<remarks>Must be called after every other Graphics related object has been destroyed.</remarks>
	void Terminate();

	/// <returns>The Vulkan Instance</returns>
	[[nodiscard]] vk::Instance GetInstance();

	/// <returns>The Vulkan Physical Device in use</returns>
	[[nodiscard]] vk::PhysicalDevice GetPhysicalDevice();

	/// <returns>The Queue Family Index of the Graphics Queue</returns>
	[[nodiscard]] uint32_t GetGraphicsQueueFamily();
	[[nodiscard]] vk::Queue GetGraphicsQueue();

	/// <returns>The Vulkan Device in use</returns>
	[[nodiscard]] vk::Device GetDevice();

	/// <summary>
	/// Blocks until the Vulkan Device is idling, must be called before destroying e.g. a swapchain.
	/// </summary>
	void WaitIdle();

}

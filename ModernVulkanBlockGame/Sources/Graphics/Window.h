#pragma once

typedef struct GLFWwindow GLFWwindow;

#include <vulkan/vulkan.hpp>

namespace Graphics {

	class Window {
	public:
		/// <summary>
		/// Creates an empty window object that does not refer to a window.
		/// </summary>
		Window();

		/// <summary>
		/// Creates a new window object refering to a newly created window.
		/// </summary>
		/// <param name="w">The width of the window content area</param>
		/// <param name="h">The height of the window content area</param>
		/// <param name="title">The title of the window</param>
		Window(int w, int h, const char* title);

		Window(const Window&) = delete;
		void operator=(const Window&) = delete;

		Window(Window&& r) noexcept;
		Window& operator=(Window&& r) noexcept;

		~Window();
		/// <summary>
		/// Destroys a window if the given object refers to a valid window.
		/// </summary>
		void Destroy();
		
		/// <returns>True if the given Window object refers to a valid window.</returns>
		[[nodiscard]] bool IsValid() const;
		operator bool() const;

		/// <returns>True if the window that this object refers to was closed.</returns>
		[[nodiscard]] bool Closed() const;

		[[nodiscard]] auto GetSurface() const { return m_Surface; }
		[[nodiscard]] auto GetSwapchain() const { return m_Swapchain; }
		[[nodiscard]] auto GetColorSpace() const { return m_Format.colorSpace; }
		[[nodiscard]] auto GetFormat() const { return m_Format.format; }
		[[nodiscard]] auto GetExtent() const { return m_SwapchainExtent; }
		[[nodiscard]] const auto& GetImages() const { return m_SwapchainImages; }
		[[nodiscard]] const auto& GetImageViews() const { return m_SwapchainImageViews; }

		void ResizeSwapchain();

		/// <summary>
		/// Updates all Window Events.
		/// </summary>
		static void UpdateAll();

	private:
		GLFWwindow* m_Window;
		vk::SurfaceKHR m_Surface;
		vk::SwapchainKHR m_Swapchain;
		vk::SurfaceFormatKHR m_Format;
		vk::Extent2D m_SwapchainExtent;
		std::vector<vk::Image> m_SwapchainImages;
		std::vector<vk::ImageView> m_SwapchainImageViews;
	};

}

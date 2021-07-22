#include "Window.h"

#include <GLFW/glfw3.h>
#include <algorithm>

#include "Logging/Log.h"
#include "Manager.h"

namespace Graphics {

	Window::Window()
		: m_Window{nullptr}, m_Surface{nullptr}, m_Swapchain{nullptr}
	{ }

	Window::Window(int w, int h, const char* title)
		: m_Window{nullptr}, m_Surface{nullptr}
	{
		if(glfwInit() != GLFW_TRUE) {
			const char* msg;
			glfwGetError(&msg);
			Log::Error("Failed to init GLFW: {}", msg);
			return;
		}

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
		glfwWindowHint(GLFW_VISIBLE, GLFW_TRUE);
		glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

		m_Window = glfwCreateWindow(w, h, title, nullptr, nullptr);
		if(m_Window == nullptr) {
			const char* msg;
			glfwGetError(&msg);
			Log::Error("Failed to create Window: {}", msg);
			return;
		}

		VkSurfaceKHR surface;
		auto err = glfwCreateWindowSurface(Manager::GetInstance(), m_Window, nullptr, &surface);
		if(err != VK_SUCCESS) {
			Log::Error("Failed to create window vulkan surface");
			return;
		}
		m_Surface = surface;

		/*
		 * We double check here that the selected graphics queue family supports presenting to the window surface.
		 * On every platform I know of, the check in Manager::IsPhysicalDeviceSuitable() de-facto guarantees that this is always true,
		 * but the Vulkan spec does not guarantee this, which is why the validation layers will complain if we do not include this check.
		 */
		auto supported = Manager::GetPhysicalDevice().getSurfaceSupportKHR(Manager::GetGraphicsQueueFamily(), m_Surface);
		if(!supported) {
			Log::Error("Window surface not supported by graphics queue family");
			return;
		}

		auto caps = Manager::GetPhysicalDevice().getSurfaceCapabilitiesKHR(m_Surface);
		auto formats = Manager::GetPhysicalDevice().getSurfaceFormatsKHR(m_Surface);
		auto modes = Manager::GetPhysicalDevice().getSurfacePresentModesKHR(m_Surface);

		vk::SurfaceFormatKHR swapchainFormat;
		for(const auto& fmt : formats) {
			if(fmt.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear && (fmt.format == vk::Format::eR8G8B8A8Srgb || fmt.format == vk::Format::eB8G8R8A8Srgb)) {
				swapchainFormat = fmt;
				break;
			}
		}
		if(swapchainFormat.format == vk::Format::eUndefined) {
			Log::Error("No suitable swapchain format supported");
			return;
		}
		m_Format = swapchainFormat;

		// On windows and X11, currentExtent will never be (UINT32_MAX, UINT32_MAX).
		m_SwapchainExtent = caps.currentExtent;

		// Select apropriate swapchain image count
		uint32_t imageCount = 3;
		if (imageCount < caps.minImageCount)
			imageCount = caps.minImageCount;
		// maxImageCount may be zero when there is no max image count, so we need an extra check.
		if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
			imageCount = caps.maxImageCount;

		// Fifo must always be supported, so we use this as default.
		vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
		for(const auto& mode : modes) {
			// Mailbox is our most preferred mode, if supported we just break.
			if(mode == vk::PresentModeKHR::eMailbox) {
				presentMode = mode;
				break;
			}
			if(mode == vk::PresentModeKHR::eImmediate) {
				presentMode = mode;
			}
		}

		vk::SwapchainCreateInfoKHR swapchainInfo{
			{}, m_Surface, imageCount,
			m_Format.format, m_Format.colorSpace,
			m_SwapchainExtent, 1,
			vk::ImageUsageFlagBits::eColorAttachment | vk::ImageUsageFlagBits::eTransferSrc,
			vk::SharingMode::eExclusive, {},
			caps.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque,
			presentMode,
			true,
			nullptr
		};
		m_Swapchain = Manager::GetDevice().createSwapchainKHR(swapchainInfo);

		m_SwapchainImages = Manager::GetDevice().getSwapchainImagesKHR(m_Swapchain);
		Log::Info("Using {} swapchain images", m_SwapchainImages.size());

		vk::ImageViewCreateInfo viewInfo{
			{}, nullptr, vk::ImageViewType::e2D,
			m_Format.format, {},
			vk::ImageSubresourceRange {
				vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1
			}
		};
		m_SwapchainImageViews.reserve(m_SwapchainImages.size());
		for(const auto& img : m_SwapchainImages) {
			viewInfo.image = img;
			m_SwapchainImageViews.push_back(Manager::GetDevice().createImageView(viewInfo));
		}
	}

	Window::Window(Window&& r) noexcept
		: m_Window{std::exchange(r.m_Window, nullptr)},
	      m_Surface{std::exchange(r.m_Surface, nullptr)},
		  m_Swapchain{std::exchange(r.m_Swapchain, nullptr)},
		  m_Format{r.m_Format},
		  m_SwapchainExtent{r.m_SwapchainExtent},
	      m_SwapchainImages{std::move(r.m_SwapchainImages)},
	      m_SwapchainImageViews{std::move(r.m_SwapchainImageViews)}
	{ }

	Window& Window::operator=(Window&& r) noexcept {
		std::swap(m_Window, r.m_Window);
		std::swap(m_Surface, r.m_Surface);
		std::swap(m_Swapchain, r.m_Swapchain);
		m_SwapchainImages = std::move(r.m_SwapchainImages);
		m_SwapchainImageViews = std::move(r.m_SwapchainImageViews);
		m_Format = r.m_Format;
		m_SwapchainExtent = r.m_SwapchainExtent;
		return *this;
	}

	Window::~Window() {
		Destroy();
	}

	void Window::Destroy() {
		if(IsValid()) {
			for (const auto& v : m_SwapchainImageViews)
				Manager::GetDevice().destroyImageView(v);

			Manager::GetDevice().destroySwapchainKHR(m_Swapchain);
			Manager::GetInstance().destroySurfaceKHR(m_Surface);

			glfwDestroyWindow(m_Window);
			m_Window = nullptr;
		}
	}

	bool Window::IsValid() const {
		return m_Window != nullptr;
	}

	Window::operator bool() const {
		return IsValid();
	}

	bool Window::Closed() const {
		return glfwWindowShouldClose(m_Window);
	}

	void Window::ResizeSwapchain() {
		// Destroy old ImageViews
		for (const auto& v : m_SwapchainImageViews)
			Manager::GetDevice().destroyImageView(v);
		m_SwapchainImageViews.clear();

		auto caps = Manager::GetPhysicalDevice().getSurfaceCapabilitiesKHR(m_Surface);
		auto modes = Manager::GetPhysicalDevice().getSurfacePresentModesKHR(m_Surface);

		m_SwapchainExtent = caps.currentExtent;

		uint32_t imageCount = 3;
		if (imageCount < caps.minImageCount)
			imageCount = caps.minImageCount;
		if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
			imageCount = caps.maxImageCount;

		vk::PresentModeKHR presentMode = vk::PresentModeKHR::eFifo;
		for (const auto& mode : modes) {
			if (mode == vk::PresentModeKHR::eMailbox) {
				presentMode = mode;
				break;
			}
			if (mode == vk::PresentModeKHR::eImmediate) {
				presentMode = mode;
			}
		}

		vk::SwapchainCreateInfoKHR swapchainInfo{
			{}, m_Surface, imageCount,
			m_Format.format, m_Format.colorSpace,
			m_SwapchainExtent, 1,
			vk::ImageUsageFlagBits::eColorAttachment,
			vk::SharingMode::eExclusive, {},
			caps.currentTransform, vk::CompositeAlphaFlagBitsKHR::eOpaque,
			presentMode,
			true,
			m_Swapchain // pass old swapchain so that is gets automatically recycled.
		};
		m_Swapchain = Manager::GetDevice().createSwapchainKHR(swapchainInfo);

		m_SwapchainImages = Manager::GetDevice().getSwapchainImagesKHR(m_Swapchain);

		vk::ImageViewCreateInfo viewInfo{
			{}, nullptr, vk::ImageViewType::e2D,
			m_Format.format, {},
			vk::ImageSubresourceRange {
				vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1
			}
		};

		m_SwapchainImageViews.reserve(m_SwapchainImages.size());
		for (const auto& img : m_SwapchainImages) {
			viewInfo.image = img;
			m_SwapchainImageViews.push_back(Manager::GetDevice().createImageView(viewInfo));
		}
	}

	void Window::UpdateAll() {
		glfwPollEvents();
	}

}

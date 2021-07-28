#include "Manager.h"

#include <GLFW/glfw3.h>

#include "Logging/Log.h"

// Needed by vulkan.hpp
VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE;

namespace Graphics::Manager {

	static vk::Instance g_Instance;
	static vk::PhysicalDevice g_PhysicalDevice;
	static vk::Device g_Device;

	static uint32_t g_GraphicsQueueFamily;
	static vk::Queue g_GraphicsQueue;

	static uint32_t g_TransferQueueFamily;
	static vk::Queue g_TransferQueue;

	/// <summary>
	/// Contains the device extensions that are absolutely required.
	/// </summary>
	static const std::vector g_RequiredDeviceExtensions {
		VK_KHR_SWAPCHAIN_EXTENSION_NAME
	};

	/// <summary>
	/// Chooses a suitable Physical Device
	/// </summary>
	/// <param name="outGfxQf">On return, contains the queue family index to be used for rendering</param>
	/// <param name="outTransferQf">On return, contains the queue family index to be used for async data transfer</param>
	/// <returns>A suitable vk::PhysicalDevice or nullptr if none was found</returns>
	static vk::PhysicalDevice ChoosePhysicalDevice(uint32_t& outGfxQf, uint32_t& outTransferQf);
	/// <summary>
	/// Chooses a suitable set of instance extensions to be enabled
	/// </summary>
	/// <returns>Vector containing all instance extensions that should be enabled</returns>
	static std::vector<const char*> ChooseInstanceExtensions();

	bool Initialize() {
		// We need to initialize GLFW here in order to call glfwGetRequiredInstanceExtensions.
		if(glfwInit() != GLFW_TRUE) {
			const char* msg;
			glfwGetError(&msg);
			Log::Error("Failed to initialize GLFW: {}", msg);
			return false;
		}

		// This will load all functions needed for creating a vk::Instance.
		VULKAN_HPP_DEFAULT_DISPATCHER.init(glfwGetInstanceProcAddress);

		// Choose the instance extensions we want to use.
		auto exts = ChooseInstanceExtensions();

		/*
		 * This struct is mainly used to specify which Vulkan API version our application is designed to use.
		 * The application name, version and engine name, version is probably completely ignored by the driver.
		 * The Spec states that this info can be used by the driver to optimize certain applications.
		 */
		vk::ApplicationInfo appInfo{
			"Modern Vulkan Block Game",
			VK_MAKE_VERSION(0, 0, 1),
			"ModernVulkanEngine",
			VK_MAKE_VERSION(0, 0, 1),
			VK_API_VERSION_1_2
		};
		vk::InstanceCreateInfo instanceInfo {
			{},
			&appInfo,
			{},
			exts
		};

		try {
			g_Instance = vk::createInstance(instanceInfo);
		}
		catch (const vk::Error& e) {
			Log::Error("Failed to create vulkan instance: {}", e.what());
			return false;
		}
		// This will load all functions that are instance-specific (e.g. enumeratePhysicalDevices()).
		VULKAN_HPP_DEFAULT_DISPATCHER.init(g_Instance);

		// Choose an appropriate physical device for our application.
		g_PhysicalDevice = ChoosePhysicalDevice(g_GraphicsQueueFamily, g_TransferQueueFamily);
		if(!g_PhysicalDevice) {
			Log::Error("No suitable physical device found");
			return false;
		}

		// Retrieve and print device info for informational purposes.
		auto props = g_PhysicalDevice.getProperties2().properties;
		Log::Info("Using Physical Device {} with Vulkan Version {}.{}", props.deviceName, VK_API_VERSION_MAJOR(props.apiVersion), VK_API_VERSION_MINOR(props.apiVersion));
		Log::Info("Using Queue Family {} for graphics", g_GraphicsQueueFamily);
		Log::Info("Using Queue Family {} for async transfer", g_TransferQueueFamily);

		float dummy = 1.0f;
		/*
		 * This array contains an entry for every queue family we want to use. For an explanation of queue families, see IsPhysicalDeviceSuitable().
		 * For every queue family, we specify how many of its queues we want to use and with which priority they should be scheduled.
		 * I have never seen anyone use a priority different from 1.0f, as the impact of the priorities is pretty vaguely described in the spec.
		 */
		std::array queueInfos {
			vk::DeviceQueueCreateInfo { {}, g_GraphicsQueueFamily, 1, &dummy },
			vk::DeviceQueueCreateInfo { {}, g_TransferQueueFamily, 1, &dummy },
		};
		vk::DeviceCreateInfo devInfo{
			{},
			queueInfos,
			{},
			g_RequiredDeviceExtensions
		};
		vk::PhysicalDeviceVulkan12Features vk12Features;
		vk12Features.imagelessFramebuffer = true; // We want to use imageless framebuffers, so we need to enable that feature.
		devInfo.pNext = &vk12Features;

		try {
			g_Device = g_PhysicalDevice.createDevice(devInfo);
		} catch(const vk::Error& e) {
			Log::Error("Failed to create vulkan device: {}", e.what());
			return false;
		}
		// This will load all functions that are specific to our newly created device, e.g. createGraphicsPipeline etc.
		VULKAN_HPP_DEFAULT_DISPATCHER.init(g_Device);

		// Here we retrieve handles to the queues we specified above.
		g_GraphicsQueue = g_Device.getQueue(g_GraphicsQueueFamily, 0);
		g_TransferQueue = g_Device.getQueue(g_TransferQueueFamily, 0);

		return true;
	}

	void Terminate() {
		g_Device.destroy();
		g_Instance.destroy();
	}

	vk::Instance GetInstance() {
		return g_Instance;
	}

	vk::PhysicalDevice GetPhysicalDevice() {
		return g_PhysicalDevice;
	}

	uint32_t GetGraphicsQueueFamily() {
		return g_GraphicsQueueFamily;
	}

	vk::Queue GetGraphicsQueue() {
		return g_GraphicsQueue;
	}

	vk::Device GetDevice() {
		return g_Device;
	}

	void WaitIdle() {
		g_Device.waitIdle();
	}

	static std::vector<const char*> ChooseInstanceExtensions() {
		// First, query all extensions required by GLFW.
		uint32_t numGlfwExts;
		auto glfwExts = glfwGetRequiredInstanceExtensions(&numGlfwExts);

		// Since every GLFW extension is required, we just push it into the result vector.
		std::vector<const char*> exts;
		for(uint32_t i = 0; i < numGlfwExts; i++) {
			exts.push_back(glfwExts[i]);
		}

		// retrieve information about all the extensions our instance supports.
		auto props = vk::enumerateInstanceExtensionProperties();
		for(const auto& p : props) {
			// If the swapchain_color_space extension is supported, we want to enable it.
			if(strcmp(p.extensionName, VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME) == 0) {
				exts.push_back(VK_EXT_SWAPCHAIN_COLOR_SPACE_EXTENSION_NAME);
				Log::Info("Instance supports HDR");
			}
		}

		return exts;
	}

	static bool IsPhysicalDeviceSuitable(vk::PhysicalDevice physDev, uint32_t& outGfxQf, uint32_t& outTransferQf) {
		auto props = physDev.getProperties2().properties;
		auto featureChain = physDev.getFeatures2<vk::PhysicalDeviceFeatures2, vk::PhysicalDeviceVulkan12Features>();
		const auto& vk12Features = featureChain.get<vk::PhysicalDeviceVulkan12Features>();

		// Device must support Vulkan 1.2
		if (props.apiVersion < VK_API_VERSION_1_2)
			return false;

		// Device must be a discrete GPU. Just a simple hack to not use my integrated GPU as it sucks.
		// Normally, a user should be able to select from a list of suitable GPUs.
		if (props.deviceType != vk::PhysicalDeviceType::eDiscreteGpu)
			return false;

		// Device must support imageless framebuffers. Any GPU I know of supports this.
		if (!vk12Features.imagelessFramebuffer)
			return false;

		// Device must support every extension contained in g_RequiredDeviceExtensions.
		auto extensions = physDev.enumerateDeviceExtensionProperties();
		for(auto reqExt : g_RequiredDeviceExtensions) {
			bool found = false;
			for(const auto& ext : extensions) {
				if(strcmp(ext.extensionName, reqExt) == 0) {
					found = true;
					break;
				}
			}

			if (!found)
				return false;
		}

		/*
		 * In the following code, we choose "queue families" which we want to use. A Queue family is a collection of at least one queue that share the same usage properties.
		 * Think of a queue as a hardware line through which we can send commands to a Vulkan Device. Each queue operates mostly independent of the other queues, meaning that
		 * they can be used from different threads etc. This is one huge advantage over OpenGL, where we only have a context that can be accessed from a single thread only.
		 * With Vulkan, we choose which queue should receive commands from which application thread.
		 * 
		 * What commands we can issue through a specific queue is specified by its queue family. For example, a queue family that supports only compute commands will not be able
		 * to receive any graphics related commands.
		 *
		 * Below we search for a queue family that supports graphics commands as well as presenting images to a swapchain. This will be called our graphics queue.
		 * Additionally, we search for a queue that only supports transfer commands (e.g. buffer copy commands), since such a queue is very good for background loading
		 * on desktop hardware.
		 */
		
		auto qFamilies = physDev.getQueueFamilyProperties();
		uint32_t gfxQueueFamily = -1;
		uint32_t transferQueueFamily = -1;

		for (uint32_t i = 0; i < qFamilies.size(); i++) {
			const auto& qf = qFamilies[i];

			// If a queue family supports Graphics and presenting images to swapchains, we can use it as our graphics queue.
			if(gfxQueueFamily == -1 && qf.queueFlags & vk::QueueFlagBits::eGraphics && glfwGetPhysicalDevicePresentationSupport(g_Instance, physDev, i)) {
				gfxQueueFamily = i;
				continue;
			}

			// On desktop hardware (at least AMD and NVIDIA), a queue that only supports transfer represents async DMA transfer lanes that are extremely good for background data transfer.
			if(transferQueueFamily == -1 && qf.queueFlags & vk::QueueFlagBits::eTransfer && !(qf.queueFlags & vk::QueueFlagBits::eGraphics) && !(qf.queueFlags & vk::QueueFlagBits::eCompute)) {
				transferQueueFamily = i;
				continue;
			}
		}

		// If we haven't found a graphics or transfer queue, this device is not suitable.
		if (gfxQueueFamily == -1 || transferQueueFamily == -1)
			return false;

		outGfxQf = gfxQueueFamily;
		outTransferQf = transferQueueFamily;

		return true;
	}

	static vk::PhysicalDevice ChoosePhysicalDevice(uint32_t& outGfxQf, uint32_t& outTransferQf) {
		// retrieve information about every physical device our instance knows of.
		auto devs = g_Instance.enumeratePhysicalDevices();
		for(const auto& dev : devs) {
			if (IsPhysicalDeviceSuitable(dev, outGfxQf, outTransferQf))
				return dev;
		}

		return nullptr;
	}

}

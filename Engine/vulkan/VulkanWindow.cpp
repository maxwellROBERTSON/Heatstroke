#include "VulkanWindow.hpp"

// Adapted from: COMP5892M (Advanced Rendering)

#include <algorithm>
#include <cassert>
#include <cstdio>
#include <limits>
#include <memory>
#include <optional>
#include <tuple>
#include <unordered_set>
#include <utility>
#include <vector>

#include <volk/volk.h>

#include "ContextHelper.hpp"
#include "Error.hpp"
#include "PipelineCreation.hpp"
#include "toString.hpp"
#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"

#include "../Input/InputCodes.hpp"

#if !defined(GLFW_INCLUDE_NONE)
#define GLFW_INCLUDE_NONE 1
#endif
#include <GLFW/glfw3.h>

namespace Engine {

	VulkanWindow::~VulkanWindow()
	{
		// Device-related objects
		for (auto const view : swapViews)
			vkDestroyImageView(device->device, view, nullptr);

		if (VK_NULL_HANDLE != swapchain) {
			vkDestroySwapchainKHR(device->device, swapchain, nullptr);
		}

		// Window and related objects
		if (VK_NULL_HANDLE != surface) {
			vkDestroySurfaceKHR(instance, surface, nullptr);
		}

		if (window)
		{
			glfwDestroyWindow(window);

			// The following assumes that we never create more than one window;
			// if there are multiple windows, destroying one of them would
			// unload the whole GLFW library. Nevertheless, this solution is
			// convenient when only dealing with one window (which we will do
			// in the exercises), as it ensure that GLFW is unloaded after all
			// window-related resources are.
			// (We also will assume this for our project)
			glfwTerminate();
		}

		if (device->dPool != VK_NULL_HANDLE) {
			vkDestroyDescriptorPool(device->device, device->dPool, nullptr);
		}

		if (device->cPool != VK_NULL_HANDLE) {
			vkDestroyCommandPool(device->device, device->cPool, nullptr);
		}

		if (device->device != VK_NULL_HANDLE) {
			vkDestroyDevice(device->device, nullptr);
		}

		if (debugMessenger != VK_NULL_HANDLE) {
			vkDestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
		}

		if (instance != VK_NULL_HANDLE) {
			vkDestroyInstance(instance, nullptr);
		}
	}

	VulkanWindow::VulkanWindow(VulkanWindow&& aOther) noexcept
		: instance(std::exchange(aOther.instance, VK_NULL_HANDLE))
		, physicalDevice(std::exchange(aOther.physicalDevice, VK_NULL_HANDLE))
		, device(std::exchange(aOther.device, VK_NULL_HANDLE)) // This line is probably wrong
		, graphicsFamilyIndex(aOther.graphicsFamilyIndex)
		, graphicsQueue(std::exchange(aOther.graphicsQueue, VK_NULL_HANDLE))
		, debugMessenger(std::exchange(aOther.debugMessenger, VK_NULL_HANDLE))
		, window(std::exchange(aOther.window, VK_NULL_HANDLE))
		, surface(std::exchange(aOther.surface, VK_NULL_HANDLE))
		, presentFamilyIndex(aOther.presentFamilyIndex)
		, presentQueue(std::exchange(aOther.presentQueue, VK_NULL_HANDLE))
		, swapchain(std::exchange(aOther.swapchain, VK_NULL_HANDLE))
		, swapImages(std::move(aOther.swapImages))
		, swapViews(std::move(aOther.swapViews))
		, swapchainFormat(aOther.swapchainFormat)
		, swapchainExtent(aOther.swapchainExtent)
	{
	}

	VulkanWindow& VulkanWindow::operator=(VulkanWindow&& aOther) noexcept
	{
		std::swap(instance, aOther.instance);
		std::swap(physicalDevice, aOther.physicalDevice);
		std::swap(device, aOther.device);
		std::swap(graphicsFamilyIndex, aOther.graphicsFamilyIndex);
		std::swap(graphicsQueue, aOther.graphicsQueue);
		std::swap(debugMessenger, aOther.debugMessenger);
		std::swap(window, aOther.window);
		std::swap(surface, aOther.surface);
		std::swap(presentFamilyIndex, aOther.presentFamilyIndex);
		std::swap(presentQueue, aOther.presentQueue);
		std::swap(swapchain, aOther.swapchain);
		std::swap(swapImages, aOther.swapImages);
		std::swap(swapViews, aOther.swapViews);
		std::swap(swapchainFormat, aOther.swapchainFormat);
		std::swap(swapchainExtent, aOther.swapchainExtent);
		return *this;
	}

	std::unique_ptr<VulkanWindow> initialiseVulkan(const std::string& name, int width, int height) {
		std::unique_ptr<VulkanWindow> window = std::make_unique<VulkanWindow>();

		// Initialize Volk
		if (auto const res = volkInitialize(); VK_SUCCESS != res)
		{
			throw Utils::Error("Unable to load Vulkan API\n"
				"Volk returned error %s", Utils::toString(res).c_str()
			);
		}

		if (GLFW_TRUE != glfwInit()) {
			const char* errMsg = nullptr;
			glfwGetError(&errMsg);

			throw Utils::Error("GLFW initialisation failed: %s", errMsg);
		}

		if (!glfwVulkanSupported())
			throw Utils::Error("GLFW: Vulkan not supported");

		// Check for instance layers and extensions
		auto const supportedLayers = Utils::getInstanceLayers();
		auto const supportedExtensions = Utils::getInstanceExtensions();

		bool enableDebugUtils = false;

		std::vector<char const*> enabledLayers, enabledExensions;

		std::uint32_t reqExtCount = 0;
		const char** requiredExt = glfwGetRequiredInstanceExtensions(&reqExtCount);

		for (std::uint32_t i = 0; i < reqExtCount; ++i) {
			if (!supportedExtensions.count(requiredExt[i]))
				throw Utils::Error("GLFW/Vulkan: Required instance extension %s not supported", requiredExt[i]);

			enabledExensions.emplace_back(requiredExt[i]);
		}

		// Validation layers support.
#		if !defined(NDEBUG) // debug builds only
		if (supportedLayers.count("VK_LAYER_KHRONOS_validation"))
		{
			enabledLayers.emplace_back("VK_LAYER_KHRONOS_validation");
		}

		if (supportedExtensions.count("VK_EXT_debug_utils"))
		{
			enableDebugUtils = true;
			enabledExensions.emplace_back("VK_EXT_debug_utils");
		}
#		endif // ~ debug builds

		for (auto const& layer : enabledLayers)
			std::fprintf(stderr, "Enabling layer: %s\n", layer);

		for (auto const& extension : enabledExensions)
			std::fprintf(stderr, "Enabling instance extension: %s\n", extension);

		// Create Vulkan instance
		window.get()->instance = Utils::createInstance(enabledLayers, enabledExensions, enableDebugUtils);

		// Load rest of the Vulkan API
		volkLoadInstance(window.get()->instance);

		// Setup debug messenger
		if (enableDebugUtils)
			window.get()->debugMessenger = Utils::createDebugMessenger(window.get()->instance);

		// Create GLFW window
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		window.get()->window = glfwCreateWindow(width, height, name.c_str(), nullptr, nullptr); // Again, name not final
		if (!window.get()->window) {
			const char* errMsg = nullptr;
			glfwGetError(&errMsg);

			throw Utils::Error("Unable to create GLFW window\n Last error = %s", errMsg);
		}

		// Set window user pointer (for events)
		glfwSetWindowUserPointer(window.get()->window, window.get());

		// Register Callbacks
		//window.get()->RegisterCallbacks();

		// Get VkSurfaceKHR from the window
		if (const auto res = glfwCreateWindowSurface(window.get()->instance, window.get()->window, nullptr, &window.get()->surface); VK_SUCCESS != res)
			throw Utils::Error("Unable to create VkSurfaceKHR\n glfwCreateWindowSurface() returned %s", Utils::toString(res).c_str());

		// Select appropriate Vulkan device
		window.get()->physicalDevice = selectDevice(window.get()->instance, window.get()->surface);
		if (VK_NULL_HANDLE == window.get()->physicalDevice)
			throw Utils::Error("No suitable physical device found!");

		VkDeviceSize minUBOAlignment;
		VkSampleCountFlagBits maxSampleCount = VK_SAMPLE_COUNT_1_BIT;


		{
			VkPhysicalDeviceProperties props;
			vkGetPhysicalDeviceProperties(window.get()->physicalDevice, &props);
			std::fprintf(stderr, "Selected device: %s (%d.%d.%d)\n", props.deviceName, VK_API_VERSION_MAJOR(props.apiVersion), VK_API_VERSION_MINOR(props.apiVersion), VK_API_VERSION_PATCH(props.apiVersion));
			minUBOAlignment = props.limits.minUniformBufferOffsetAlignment;
		
			VkSampleCountFlags supportedSampleCount = std::min(props.limits.framebufferColorSampleCounts, props.limits.framebufferDepthSampleCounts);
			std::vector<VkSampleCountFlagBits> possibleSampleCounts = { VK_SAMPLE_COUNT_64_BIT, VK_SAMPLE_COUNT_32_BIT, VK_SAMPLE_COUNT_16_BIT, VK_SAMPLE_COUNT_8_BIT, VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_2_BIT };
			for (VkSampleCountFlagBits possibleSampleCount : possibleSampleCounts) {
				if (supportedSampleCount & possibleSampleCount) {
					maxSampleCount = possibleSampleCount;
					break;
				}
			}
		}

		// Create a logical device
		// Enable required extensions. The device selection method ensures that
		// the VK_KHR_swapchain extension is present, so we can safely just
		// request it without further checks.
		std::vector<char const*> enabledDevExensions;

		enabledDevExensions.emplace_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);

		for (auto const& ext : enabledDevExensions)
			std::fprintf(stderr, "Enabling device extension: %s\n", ext);

		// We need one or two queues:
		// - best case: one GRAPHICS queue that can present
		// - otherwise: one GRAPHICS queue and any queue that can present
		std::vector<std::uint32_t> queueFamilyIndices;

		if (const auto index = findQueueFamily(window.get()->physicalDevice, VK_QUEUE_GRAPHICS_BIT, window.get()->surface)) {
			window.get()->graphicsFamilyIndex = *index;

			queueFamilyIndices.emplace_back(*index);
		}
		else {
			auto graphics = findQueueFamily(window.get()->physicalDevice, VK_QUEUE_GRAPHICS_BIT);
			auto present = findQueueFamily(window.get()->physicalDevice, 0, window.get()->surface);

			assert(graphics && present);

			window.get()->graphicsFamilyIndex = *graphics;
			window.get()->presentFamilyIndex = *present;

			queueFamilyIndices.emplace_back(*graphics);
			queueFamilyIndices.emplace_back(*present);
		}

		window.get()->device = createDevice(*window.get(), window.get()->physicalDevice, queueFamilyIndices, enabledDevExensions);

		window.get()->device->minUBOAlignment = minUBOAlignment;
		window.get()->device->maxSampleCount = maxSampleCount;

		// Retrieve VkQueues
		vkGetDeviceQueue(window.get()->device->device, window.get()->graphicsFamilyIndex, 0, &window.get()->graphicsQueue);

		assert(VK_NULL_HANDLE != window.get()->graphicsQueue);

		if (queueFamilyIndices.size() >= 2)
			vkGetDeviceQueue(window.get()->device->device, window.get()->presentFamilyIndex, 0, &window.get()->presentQueue);
		else
		{
			window.get()->presentFamilyIndex = window.get()->graphicsFamilyIndex;
			window.get()->presentQueue = window.get()->graphicsQueue;
		}

		// Create swap chain
		std::tie(window.get()->swapchain, window.get()->swapchainFormat, window.get()->swapchainExtent) = createSwapchain(window.get()->physicalDevice, window.get()->surface, window.get()->device->device, window.get()->window, queueFamilyIndices);

		// Get swap chain images & create associated image views
		getSwapchainImages(window.get()->device->device, window.get()->swapchain, window.get()->swapImages);
		createSwapchainImageViews(window.get()->device->device, window.get()->swapchainFormat, window.get()->swapImages, window.get()->swapViews);

		// Done
		return window;
	}

	SwapChanges recreateSwapchain(VulkanWindow& aWindow, VkPresentModeKHR desiredPresentMode) {
		const auto oldFormat = aWindow.swapchainFormat;
		const auto oldExtent = aWindow.swapchainExtent;

		VkSwapchainKHR oldSwapchain = aWindow.swapchain;

		for (auto view : aWindow.swapViews)
			vkDestroyImageView(aWindow.device->device, view, nullptr);

		aWindow.swapViews.clear();
		aWindow.swapImages.clear();

		std::vector<std::uint32_t> queueFamilyIndices;
		if (aWindow.presentFamilyIndex != aWindow.graphicsFamilyIndex) {
			queueFamilyIndices.emplace_back(aWindow.graphicsFamilyIndex);
			queueFamilyIndices.emplace_back(aWindow.presentFamilyIndex);
		}

		try {
			std::tie(aWindow.swapchain, aWindow.swapchainFormat, aWindow.swapchainExtent) =
				createSwapchain(aWindow.physicalDevice, aWindow.surface, aWindow.device->device, aWindow.window, queueFamilyIndices, oldSwapchain, desiredPresentMode);
		}
		catch (...) {
			aWindow.swapchain = oldSwapchain;
			throw;
		}

		vkDestroySwapchainKHR(aWindow.device->device, oldSwapchain, nullptr);

		getSwapchainImages(aWindow.device->device, aWindow.swapchain, aWindow.swapImages);
		createSwapchainImageViews(aWindow.device->device, aWindow.swapchainFormat, aWindow.swapImages, aWindow.swapViews);

		SwapChanges ret{};

		if (oldExtent.width != aWindow.swapchainExtent.width || oldExtent.height != aWindow.swapchainExtent.height)
			ret.changedSize = true;
		if (oldFormat != aWindow.swapchainFormat)
			ret.changedFormat = true;

		return ret;
	}

	VkPhysicalDevice selectDevice(VkInstance aInstance, VkSurfaceKHR aSurface) {
		std::uint32_t numDevices = 0;
		if (auto const res = vkEnumeratePhysicalDevices(aInstance, &numDevices, nullptr); VK_SUCCESS != res)
		{
			throw Utils::Error("Unable to get physical device count\n"
				"vkEnumeratePhysicalDevices() returned %s", Utils::toString(res).c_str()
			);
		}

		std::vector<VkPhysicalDevice> devices(numDevices, VK_NULL_HANDLE);
		if (auto const res = vkEnumeratePhysicalDevices(aInstance, &numDevices, devices.data()); VK_SUCCESS != res)
		{
			throw Utils::Error("Unable to get physical device list\n"
				"vkEnumeratePhysicalDevices() returned %s", Utils::toString(res).c_str()
			);
		}

		float bestScore = -1.f;
		VkPhysicalDevice bestDevice = VK_NULL_HANDLE;

		for (auto const device : devices)
		{
			auto const score = scoreDevice(device, aSurface);
			if (score > bestScore)
			{
				bestScore = score;
				bestDevice = device;
			}
		}

		return bestDevice;
	}
	float scoreDevice(VkPhysicalDevice aPhysicalDev, VkSurfaceKHR aSurface) {
		VkPhysicalDeviceProperties props;
		vkGetPhysicalDeviceProperties(aPhysicalDev, &props);

		// Only consider Vulkan 1.1 devices
		auto const major = VK_API_VERSION_MAJOR(props.apiVersion);
		auto const minor = VK_API_VERSION_MINOR(props.apiVersion);

		if (major < 1 || (major == 1 && minor < 2))
		{
			std::fprintf(stderr, "Info: Discarding device '%s': insufficient vulkan version\n", props.deviceName);
			return -1.f;
		}

		const auto exts = Utils::getDeviceExtensions(aPhysicalDev);

		if (!exts.count(VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
			std::fprintf(stderr, "Info: Discarding device '%s': extension %s missing\n", props.deviceName, VK_KHR_SWAPCHAIN_EXTENSION_NAME);
			return -1.0f;
		}

		if (!findQueueFamily(aPhysicalDev, 0, aSurface)) {
			std::fprintf(stderr, "Info: Discarding device '%s': can't present to surface\n", props.deviceName);
			return -1.0f;
		}

		if (!findQueueFamily(aPhysicalDev, VK_QUEUE_GRAPHICS_BIT)) {
			std::fprintf(stderr, "Info: Discarding device '%s': no graphics queue family\n", props.deviceName);
			return -1.0f;
		}

		// Discrete GPU > Integrated GPU > others
		float score = 0.f;

		if (VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU == props.deviceType)
			score += 500.f;
		else if (VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU == props.deviceType)
			score += 100.f;

		return score;
	}

	// Note: this finds *any* queue that supports the aQueueFlags. As such,
	//   find_queue_family( ..., VK_QUEUE_TRANSFER_BIT, ... );
	// might return a GRAPHICS queue family, since GRAPHICS queues typically
	// also set TRANSFER (and indeed most other operations; GRAPHICS queues are
	// required to support those operations regardless). If you wanted to find
	// a dedicated TRANSFER queue (e.g., such as those that exist on NVIDIA
	// GPUs), you would need to use different logic.
	std::optional<std::uint32_t> findQueueFamily(VkPhysicalDevice aPhysicalDev, VkQueueFlags aQueueFlags, VkSurfaceKHR aSurface) {
		std::uint32_t numQueues = 0;
		vkGetPhysicalDeviceQueueFamilyProperties(aPhysicalDev, &numQueues, nullptr);

		std::vector<VkQueueFamilyProperties> families(numQueues);
		vkGetPhysicalDeviceQueueFamilyProperties(aPhysicalDev, &numQueues, families.data());

		for (std::uint32_t i = 0; i < numQueues; ++i) {
			const auto& family = families[i];

			if (aQueueFlags == (aQueueFlags & family.queueFlags)) {
				if (VK_NULL_HANDLE == aSurface)
					return i;

				VkBool32 supported = VK_FALSE;
				const auto res = vkGetPhysicalDeviceSurfaceSupportKHR(aPhysicalDev, i, aSurface, &supported);

				if (VK_SUCCESS == res && supported)
					return i;
			}
		}

		return {};
	}

	std::unique_ptr<VulkanDevice> createDevice(const VulkanWindow& aWindow, VkPhysicalDevice aPhysicalDev, std::vector<std::uint32_t> const& aQueues, std::vector<char const*> const& aEnabledExtensions) {
		if (aQueues.empty())
			throw Utils::Error("createDevice(): no queues requested");

		float queuePriorities[1] = { 1.f };

		std::vector<VkDeviceQueueCreateInfo> queueInfos(aQueues.size());
		for (std::size_t i = 0; i < aQueues.size(); ++i)
		{
			auto& queueInfo = queueInfos[i];
			queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
			queueInfo.queueFamilyIndex = aQueues[i];
			queueInfo.queueCount = 1;
			queueInfo.pQueuePriorities = queuePriorities;
		}

		VkPhysicalDeviceFeatures deviceFeatures{};
		// No extra features for now.

		VkDeviceCreateInfo deviceInfo{};
		deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

		deviceInfo.queueCreateInfoCount = std::uint32_t(queueInfos.size());
		deviceInfo.pQueueCreateInfos = queueInfos.data();

		deviceInfo.enabledExtensionCount = std::uint32_t(aEnabledExtensions.size());
		deviceInfo.ppEnabledExtensionNames = aEnabledExtensions.data();

		deviceInfo.pEnabledFeatures = &deviceFeatures;

		VkDevice device = VK_NULL_HANDLE;
		if (auto const res = vkCreateDevice(aPhysicalDev, &deviceInfo, nullptr, &device); VK_SUCCESS != res)
		{
			throw Utils::Error("Unable to create logical device\n"
				"vkCreateDevice() returned %s", Utils::toString(res).c_str()
			);
		}

		return std::make_unique<VulkanDevice>(device, aWindow);
	}

	std::vector<VkSurfaceFormatKHR> getSurfaceFormats(VkPhysicalDevice aPhysicalDev, VkSurfaceKHR aSurface) {
		std::uint32_t numFormats = 0;
		if (const auto res = vkGetPhysicalDeviceSurfaceFormatsKHR(aPhysicalDev, aSurface, &numFormats, nullptr); VK_SUCCESS != res)
			throw Utils::Error("Unable to get surface formats\n vkGetPhysicalDeviceSurfaceFormatsKHR() returned %s", Utils::toString(res).c_str());

		std::vector<VkSurfaceFormatKHR> formats(numFormats);
		if (const auto res = vkGetPhysicalDeviceSurfaceFormatsKHR(aPhysicalDev, aSurface, &numFormats, formats.data()); VK_SUCCESS != res)
			throw Utils::Error("Unable to get surface formats\n vkGetPhysicalDeviceSurfaceFormatsKHR() returned %s", Utils::toString(res).c_str());

		return formats;
	}

	std::unordered_set<VkPresentModeKHR> getPresentModes(VkPhysicalDevice aPhysicalDev, VkSurfaceKHR aSurface) {
		std::uint32_t numModes = 0;
		if (const auto res = vkGetPhysicalDeviceSurfacePresentModesKHR(aPhysicalDev, aSurface, &numModes, nullptr); VK_SUCCESS != res)
			throw Utils::Error("Unable to get present modes\n vkGetPhysicalDeviceSurfacePresentModesKHR() returned %s", Utils::toString(res).c_str());

		std::vector<VkPresentModeKHR> modes(numModes);
		if (const auto res = vkGetPhysicalDeviceSurfacePresentModesKHR(aPhysicalDev, aSurface, &numModes, modes.data()); VK_SUCCESS != res)
			throw Utils::Error("Unable to get present modes\n vkGetPhysicalDeviceSurfacePresentModesKHR() returned %s", Utils::toString(res).c_str());

		std::unordered_set<VkPresentModeKHR> res;
		for (const auto& mode : modes) {
			res.insert(mode);
		}

		return res;
	}

	std::tuple<VkSwapchainKHR, VkFormat, VkExtent2D> createSwapchain(
		VkPhysicalDevice aPhysicalDev, 
		VkSurfaceKHR aSurface, 
		VkDevice aDevice, 
		GLFWwindow* aWindow, 
		std::vector<std::uint32_t> const& aQueueFamilyIndices, 
		VkSwapchainKHR aOldSwapchain, 
		VkPresentModeKHR desiredPresentMode) 
	{
		auto const formats = getSurfaceFormats(aPhysicalDev, aSurface);
		auto const modes = getPresentModes(aPhysicalDev, aSurface);

		VkSurfaceFormatKHR format = formats[0];
		for (const auto fmt : formats) {
			if (VK_FORMAT_R8G8B8A8_SRGB == fmt.format && VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == fmt.colorSpace) {
				format = fmt;
				break;
			}

			if (VK_FORMAT_B8G8R8A8_SRGB == fmt.format && VK_COLOR_SPACE_SRGB_NONLINEAR_KHR == fmt.colorSpace) {
				format = fmt;
				break;
			}
		}

		VkPresentModeKHR presentMode = desiredPresentMode;
		if (desiredPresentMode == VK_PRESENT_MODE_FIFO_KHR && modes.count(VK_PRESENT_MODE_FIFO_RELAXED_KHR))
			presentMode = VK_PRESENT_MODE_FIFO_RELAXED_KHR;

		VkSurfaceCapabilitiesKHR caps;
		if (const auto res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(aPhysicalDev, aSurface, &caps); VK_SUCCESS != res)
			throw Utils::Error("Unable to get surface capabilities\n vkGetPhysicalDeviceSurfaceCapabilitiesKHR() returned %s", Utils::toString(res).c_str());

		std::uint32_t imageCount = 2;

		if (imageCount < caps.minImageCount + 1)
			imageCount = caps.minImageCount + 1;

		if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
			imageCount = caps.maxImageCount;

		VkExtent2D extent = caps.currentExtent;
		if (std::numeric_limits<std::uint32_t>::max() == extent.width) {
			int width, height;
			glfwGetFramebufferSize(aWindow, &width, &height);

			const auto& min = caps.minImageExtent;
			const auto& max = caps.maxImageExtent;

			extent.width = std::clamp(std::uint32_t(width), min.width, max.width);
			extent.height = std::clamp(std::uint32_t(height), min.height, max.height);
		}

		VkSwapchainCreateInfoKHR chainInfo{};
		chainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
		chainInfo.surface = aSurface;
		chainInfo.minImageCount = imageCount;
		chainInfo.imageFormat = format.format;
		chainInfo.imageColorSpace = format.colorSpace;
		chainInfo.imageExtent = extent;
		chainInfo.imageArrayLayers = 1;
		chainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
		chainInfo.preTransform = caps.currentTransform;
		chainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
		chainInfo.presentMode = presentMode;
		chainInfo.clipped = VK_TRUE;
		chainInfo.oldSwapchain = aOldSwapchain;

		if (aQueueFamilyIndices.size() <= 1) {
			chainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		}
		else {
			chainInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
			chainInfo.queueFamilyIndexCount = std::uint32_t(aQueueFamilyIndices.size());
			chainInfo.pQueueFamilyIndices = aQueueFamilyIndices.data();
		}

		VkSwapchainKHR chain = VK_NULL_HANDLE;
		if (const auto res = vkCreateSwapchainKHR(aDevice, &chainInfo, nullptr, &chain); VK_SUCCESS != res)
			throw Utils::Error("Unable to create swap chain\n vkCreateSwapchainKHR() returned %s", Utils::toString(res).c_str());

		return { chain, format.format, extent };
	}

	void getSwapchainImages(VkDevice aDevice, VkSwapchainKHR aSwapchain, std::vector<VkImage>& aImages) {
		assert(0 == aImages.size());

		std::uint32_t numImages = 0;
		if (const auto res = vkGetSwapchainImagesKHR(aDevice, aSwapchain, &numImages, nullptr); VK_SUCCESS != res)
			throw Utils::Error("Unable to get swapchain images\n vkGetSwapchainImagesKHR() returned %s", Utils::toString(res).c_str());

		std::vector<VkImage> images(numImages);
		if (const auto res = vkGetSwapchainImagesKHR(aDevice, aSwapchain, &numImages, images.data()); VK_SUCCESS != res)
			throw Utils::Error("Unable to get swapchain images\n vkGetSwapchainImagesKHR() returned %s", Utils::toString(res).c_str());

		std::swap(aImages, images);
	}

	void createSwapchainImageViews(VkDevice aDevice, VkFormat aSwapchainFormat, std::vector<VkImage> const& aImages, std::vector<VkImageView>& aViews) {
		assert(0 == aViews.size());

		for (std::size_t i = 0; i < aImages.size(); ++i) {
			VkImageViewCreateInfo viewInfo{};
			viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
			viewInfo.image = aImages[i];
			viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
			viewInfo.format = aSwapchainFormat;
			viewInfo.components = VkComponentMapping{
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY,
				VK_COMPONENT_SWIZZLE_IDENTITY
			};
			viewInfo.subresourceRange = VkImageSubresourceRange{
				VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1
			};

			VkImageView view = VK_NULL_HANDLE;
			if (const auto res = vkCreateImageView(aDevice, &viewInfo, nullptr, &view); VK_SUCCESS != res)
				throw Utils::Error("Unable to create image view for swap chain image %zu\n vkCreateImageView() returned %s", i, Utils::toString(res).c_str());

			aViews.emplace_back(view);
		}

		assert(aViews.size() == aImages.size());
	}

	VkResult submitAndPresent(
		const VulkanWindow& aWindow,
		std::vector<VkCommandBuffer>& aCmdBuffers,
		std::vector<vk::Fence>& frameDone,
		std::vector<vk::Semaphore>& imageAvailable,
		std::vector<vk::Semaphore>& renderFinished,
		std::size_t frameIndex,
		std::uint32_t imageIndex
	) {
		// Submit
		VkPipelineStageFlags waitPipelineStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		VkSubmitInfo subInfo{};
		subInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		subInfo.commandBufferCount = 1;
		subInfo.pCommandBuffers = &aCmdBuffers[frameIndex];
		subInfo.waitSemaphoreCount = 1;
		subInfo.pWaitSemaphores = &imageAvailable[frameIndex].handle;
		subInfo.pWaitDstStageMask = &waitPipelineStages;
		subInfo.signalSemaphoreCount = 1;
		subInfo.pSignalSemaphores = &renderFinished[frameIndex].handle;

		if (const auto res = vkQueueSubmit(aWindow.graphicsQueue, 1, &subInfo, frameDone[frameIndex].handle); VK_SUCCESS != res)
			throw Utils::Error("Unable to submit command buffer to queue\n vkQueueSubmit() returned %s", Utils::toString(res).c_str());

		// Present
		VkPresentInfoKHR presentInfo{};
		presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
		presentInfo.waitSemaphoreCount = 1;
		presentInfo.pWaitSemaphores = &renderFinished[frameIndex].handle;
		presentInfo.swapchainCount = 1;
		presentInfo.pSwapchains = &aWindow.swapchain;
		presentInfo.pImageIndices = &imageIndex;
		presentInfo.pResults = nullptr;

		return vkQueuePresentKHR(aWindow.presentQueue, &presentInfo);
	}
}
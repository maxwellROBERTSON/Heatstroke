#pragma once

// Adapted from: COMP5892M (Advanced Rendering)

#include <vector>
#include <cstdint>
#include <unordered_set>
#include <optional>
#include <memory>

#include <volk/volk.h>
#if !defined(GLFW_INCLUDE_NONE)
#define GLFW_INCLUDE_NONE 1
#endif
#include <GLFW/glfw3.h>

#include "objects/VkObjects.hpp"

namespace Engine {

	class VulkanDevice;

	class VulkanWindow {
	public:
		VulkanWindow() = default;
		~VulkanWindow();

		// Move-only
		VulkanWindow(VulkanWindow const&) = delete;
		VulkanWindow& operator= (VulkanWindow const&) = delete;

		VulkanWindow(VulkanWindow&&) noexcept;
		VulkanWindow& operator= (VulkanWindow&&) noexcept;

		// Variables
		VkInstance instance = VK_NULL_HANDLE;
		VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
		std::unique_ptr<VulkanDevice> device;

		std::uint32_t graphicsFamilyIndex = 0;
		VkQueue graphicsQueue = VK_NULL_HANDLE;

		VkDebugUtilsMessengerEXT debugMessenger = VK_NULL_HANDLE;

		GLFWwindow* window = nullptr;
		VkSurfaceKHR surface = VK_NULL_HANDLE;

		std::uint32_t presentFamilyIndex = 0;
		VkQueue presentQueue = VK_NULL_HANDLE;

		VkSwapchainKHR swapchain = VK_NULL_HANDLE;
		std::vector<VkImage> swapImages;
		std::vector<VkImageView> swapViews;

		VkFormat swapchainFormat;
		VkExtent2D swapchainExtent;
	};

	struct SwapChanges {
		bool changedSize : 1;
		bool changedFormat : 1;
	};

	std::unique_ptr<VulkanWindow> initialiseVulkan();
	SwapChanges recreateSwapchain(VulkanWindow&);

	// The device selection process has changed somewhat w.r.t. the one used 
	// earlier (e.g., with VulkanContext)
	VkPhysicalDevice selectDevice(VkInstance, VkSurfaceKHR);
	float scoreDevice(VkPhysicalDevice, VkSurfaceKHR);

	std::optional<std::uint32_t> findQueueFamily(VkPhysicalDevice, VkQueueFlags, VkSurfaceKHR = VK_NULL_HANDLE);

	std::unique_ptr<VulkanDevice> createDevice(
		const VulkanWindow& aWindow,
		VkPhysicalDevice aPhysicalDev,
		std::vector<std::uint32_t> const& aQueueFamilies,
		std::vector<char const*> const& aEnabledDeviceExtensions = {}
	);

	std::vector<VkSurfaceFormatKHR> getSurfaceFormats(VkPhysicalDevice, VkSurfaceKHR);
	std::unordered_set<VkPresentModeKHR> getPresentModes(VkPhysicalDevice, VkSurfaceKHR);

	std::tuple<VkSwapchainKHR, VkFormat, VkExtent2D> createSwapchain(
		VkPhysicalDevice,
		VkSurfaceKHR,
		VkDevice,
		GLFWwindow*,
		std::vector<std::uint32_t> const& aQueueFamilyIndices = {},
		VkSwapchainKHR aOldSwapchain = VK_NULL_HANDLE
	);

	void getSwapchainImages(VkDevice, VkSwapchainKHR, std::vector<VkImage>&);
	void createSwapchainImageViews(VkDevice, VkFormat, std::vector<VkImage> const&, std::vector<VkImageView>&);

	void submitAndPresent(
		const VulkanWindow& aWindow,
		std::vector<VkCommandBuffer>& aCmdBuffers,
		std::vector<vk::Fence>& frameDone,
		std::vector<vk::Semaphore>& imageAvailable,
		std::vector<vk::Semaphore>& renderFinished,
		std::size_t frameIndex,
		std::uint32_t imageIndex
	);
}
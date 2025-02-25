#pragma once

#include <vector>

#include <volk/volk.h>

#include "objects/Model.hpp"
#include "objects/VkObjects.hpp"

namespace Engine {

	class VulkanWindow;

	class VulkanDevice {
	public:
		VulkanDevice() noexcept = default;
		~VulkanDevice() = default;

		explicit VulkanDevice(VkDevice, const VulkanWindow&) noexcept;

		// Move-only
		VulkanDevice(VulkanDevice const&) = delete;
		VulkanDevice& operator= (VulkanDevice const&) = delete;

		VulkanDevice(VulkanDevice&&) noexcept;
		VulkanDevice& operator= (VulkanDevice&&) noexcept;

		void createCommandPool(const VulkanWindow& aWindow);
		void createDescriptorPool();

		VkDevice device = VK_NULL_HANDLE;
		VkCommandPool cPool = VK_NULL_HANDLE;
		VkDescriptorPool dPool = VK_NULL_HANDLE;
	};

	VkCommandBuffer createCommandBuffer(const VulkanWindow& aWindow);
	vk::Fence createFence(const VulkanWindow& aWindow, VkFenceCreateFlags aFlags = 0);
	vk::Semaphore createSemaphore(const VulkanWindow& aWindow);

	VkDescriptorSet allocateDescriptorSet(const VulkanWindow& aWindow, VkDescriptorPool aPool, VkDescriptorSetLayout aSetLayout);

	void beginCommandBuffer(VkCommandBuffer aCmdBuff, VkCommandBufferUsageFlags aFlags = 0);
	void endAndSubmitCommandBuffer(const VulkanWindow& aWindow, VkCommandBuffer aCmdBuff);

	vk::Sampler createTextureSampler(const VulkanWindow& aWindow, vk::SamplerInfo aSamplerInfo);

	void waitForFences(const VulkanWindow& aWindow, std::vector<vk::Fence>& aFences, std::size_t frameIndex);
	void resetFences(const VulkanWindow& aWindow, std::vector<vk::Fence>& aFences, std::size_t frameIndex);

	// Returns whether swapchain should be recreated
	bool acquireNextSwapchainImage(const VulkanWindow& aWindow, std::vector<vk::Semaphore>& aSemaphores, std::size_t frameIndex, std::uint32_t& imageIndex);
}
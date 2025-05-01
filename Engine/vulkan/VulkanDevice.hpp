#pragma once

#include <vector>

#include <volk/volk.h>

#include "../gltf/Model.hpp"
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

		// Get sample count flag with a selected index
		VkSampleCountFlagBits getSampleCount(std::size_t index);

		VkDevice device = VK_NULL_HANDLE;
		VkCommandPool cPool = VK_NULL_HANDLE;
		VkDescriptorPool dPool = VK_NULL_HANDLE;

		std::size_t maxSampleCountIndex = 0;
		std::vector<VkSampleCountFlagBits> possibleSampleCounts = { VK_SAMPLE_COUNT_1_BIT, VK_SAMPLE_COUNT_2_BIT, VK_SAMPLE_COUNT_4_BIT, VK_SAMPLE_COUNT_8_BIT, VK_SAMPLE_COUNT_16_BIT, VK_SAMPLE_COUNT_32_BIT, VK_SAMPLE_COUNT_64_BIT };
		std::vector<const char*> msaaOptions = { "Disabled", "MSAA 2x", "MSAA 4x", "MSAA 8x", "MSAA 16x", "MSAA 32x", "MSAA 64x" };
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
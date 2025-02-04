#pragma once

#include <volk/volk.h>

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

		VkDevice device = VK_NULL_HANDLE;
		VkCommandPool cPool = VK_NULL_HANDLE;
	};

	VkCommandBuffer createCommandBuffer(const VulkanWindow& aWindow);
	vk::Fence createFence(const VulkanWindow& aWindow, VkFenceCreateFlags aFlags = 0);

	void beginCommandBuffer(VkCommandBuffer aCmdBuff);
	void endAndSubmitCommandBuffer(const VulkanWindow& aWindow, VkCommandBuffer aCmdBuff);

}
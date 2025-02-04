#include "VulkanDevice.hpp"

#include <utility>
#include <limits>

#include "Error.hpp"
#include "toString.hpp"
#include "VulkanWindow.hpp"

namespace Engine {

	VulkanDevice::VulkanDevice(VkDevice aDevice, const VulkanWindow& aWindow) noexcept
		: device(aDevice) 
	{
		this->createCommandPool(aWindow);
	}

	VulkanDevice::VulkanDevice(VulkanDevice&& aOther) noexcept
		: device(std::exchange(aOther.device, VK_NULL_HANDLE))
		, cPool(std::exchange(aOther.cPool, VK_NULL_HANDLE)) 
	{}

	VulkanDevice& VulkanDevice::operator=(VulkanDevice&& aOther) noexcept {
		std::swap(device, aOther.device);
		std::swap(cPool, aOther.cPool);
		return *this;
	}

	void VulkanDevice::createCommandPool(const VulkanWindow& aWindow) {
		VkCommandPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
		poolInfo.queueFamilyIndex = aWindow.graphicsFamilyIndex;
		poolInfo.flags = VK_COMMAND_POOL_CREATE_TRANSIENT_BIT | VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

		if (const auto res = vkCreateCommandPool(device, &poolInfo, nullptr, &cPool); VK_SUCCESS != res) {
			throw Utils::Error("Unable to create command pool\n vkCreateCommandPool() returned %s", Utils::toString(res).c_str());
		}
	}

	VkCommandBuffer createCommandBuffer(const VulkanWindow& aWindow) {
		VkCommandBufferAllocateInfo cbufInfo{};
		cbufInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		cbufInfo.commandPool = aWindow.device->cPool;
		cbufInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		cbufInfo.commandBufferCount = 1;

		VkCommandBuffer cbuff = VK_NULL_HANDLE;
		if (const auto res = vkAllocateCommandBuffers(aWindow.device->device, &cbufInfo, &cbuff); VK_SUCCESS != res) {
			throw Utils::Error("Unable to allocate command buffers\n vkAllocateCommandBuffers() returned %s", Utils::toString(res).c_str());
		}

		return cbuff;
	}

	vk::Fence createFence(const VulkanWindow& aWindow, VkFenceCreateFlags aFlags) {
		VkFenceCreateInfo fenceInfo{};
		fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
		fenceInfo.flags = aFlags;

		VkFence fence = VK_NULL_HANDLE;
		if (const auto res = vkCreateFence(aWindow.device->device, &fenceInfo, nullptr, &fence); VK_SUCCESS != res) {
			throw Utils::Error("Unable to create fence\n vkCreateFence() returned %s", Utils::toString(res).c_str());
		}

		return vk::Fence(aWindow.device->device, fence);
	}

	void beginCommandBuffer(VkCommandBuffer aCmdBuff) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = 0;
		beginInfo.pInheritanceInfo = nullptr;

		if (const auto res = vkBeginCommandBuffer(aCmdBuff, &beginInfo); VK_SUCCESS != res)
			throw Utils::Error("Unable to begin command buffer\n vkBeginCommandBuffer() returned %s", Utils::toString(res).c_str());
	}

	void endAndSubmitCommandBuffer(const VulkanWindow& aWindow, VkCommandBuffer aCmdBuff) {
		if (const auto res = vkEndCommandBuffer(aCmdBuff); VK_SUCCESS != res)
			throw Utils::Error("Unable to end command buffer\n vkEndCommandBuffer() returned %s", Utils::toString(res).c_str());
	
		vk::Fence uploadComplete = createFence(aWindow);

		VkSubmitInfo submitInfo{};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &aCmdBuff;

		if (const auto res = vkQueueSubmit(aWindow.graphicsQueue, 1, &submitInfo, uploadComplete.handle); VK_SUCCESS != res)
			throw Utils::Error("Unable to queue submit\n vkQueueSubmit() returned %s", Utils::toString(res).c_str());

		if (const auto res = vkWaitForFences(aWindow.device->device, 1, &uploadComplete.handle, VK_TRUE, std::numeric_limits<std::uint64_t>::max()); VK_SUCCESS != res)
			throw Utils::Error("Unable to wait for fences\n vkWaitForFences() returned %s", Utils::toString(res).c_str());

		vkFreeCommandBuffers(aWindow.device->device, aWindow.device->cPool, 1, &aCmdBuff);
	}

}
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
		this->createDescriptorPool();
	}

	VulkanDevice::VulkanDevice(VulkanDevice&& aOther) noexcept
		: device(std::exchange(aOther.device, VK_NULL_HANDLE))
		, cPool(std::exchange(aOther.cPool, VK_NULL_HANDLE))
	{
	}

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

	void VulkanDevice::createDescriptorPool() {
		const VkDescriptorPoolSize pools[] = {
			{VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, 2048},
			{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 2048}
		};

		VkDescriptorPoolCreateInfo poolInfo{};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.maxSets = 1024;
		poolInfo.poolSizeCount = sizeof(pools) / sizeof(pools[0]);
		poolInfo.pPoolSizes = pools;

		if (const auto res = vkCreateDescriptorPool(device, &poolInfo, nullptr, &dPool); VK_SUCCESS != res)
			throw Utils::Error("Unable to create descriptor pool\n vkCreateDescriptorPool() returned %s", Utils::toString(res).c_str());
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

	vk::Semaphore createSemaphore(const VulkanWindow& aWindow) {
		VkSemaphoreCreateInfo semaphoreInfo{};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

		VkSemaphore semaphore = VK_NULL_HANDLE;
		if (const auto res = vkCreateSemaphore(aWindow.device->device, &semaphoreInfo, nullptr, &semaphore); VK_SUCCESS != res)
			throw Utils::Error("Unable to create semaphore\n vkCreateSemaphore() returned %s", Utils::toString(res).c_str());

		return vk::Semaphore(aWindow.device->device, semaphore);
	}

	VkDescriptorSet allocateDescriptorSet(const VulkanWindow& aWindow, VkDescriptorPool aPool, VkDescriptorSetLayout aSetLayout) {
		VkDescriptorSetAllocateInfo allocInfo{};
		allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
		allocInfo.descriptorPool = aPool;
		allocInfo.descriptorSetCount = 1;
		allocInfo.pSetLayouts = &aSetLayout;

		VkDescriptorSet dset = VK_NULL_HANDLE;
		if (const auto res = vkAllocateDescriptorSets(aWindow.device->device, &allocInfo, &dset); VK_SUCCESS != res)
			throw Utils::Error("Unable to allocate descriptor set\n vkAllocateDescriptorSets() returned %s", Utils::toString(res).c_str());

		return dset;
	}

	void beginCommandBuffer(VkCommandBuffer aCmdBuff, VkCommandBufferUsageFlags aFlags) {
		VkCommandBufferBeginInfo beginInfo{};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = aFlags;
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

		vkFreeCommandBuffers(aWindow.device->device, aWindow.device.get()->cPool, 1, &aCmdBuff);
	}

	void createTextureSamplers(const VulkanWindow& aWindow, vk::Model& aModel, std::vector<vk::Sampler>& aSamplers) {
		for (vk::Texture& texture : aModel.textures) {
			VkSamplerCreateInfo samplerInfo{};
			samplerInfo.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
			samplerInfo.magFilter = texture.sampler > -1 ? aModel.samplerInfos[texture.sampler].magFilter : VK_FILTER_LINEAR;
			samplerInfo.minFilter = texture.sampler > -1 ? aModel.samplerInfos[texture.sampler].minFilter : VK_FILTER_LINEAR;
			samplerInfo.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
			samplerInfo.addressModeU = texture.sampler > -1 ? aModel.samplerInfos[texture.sampler].addressModeU : VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.addressModeV = texture.sampler > -1 ? aModel.samplerInfos[texture.sampler].addressModeV : VK_SAMPLER_ADDRESS_MODE_REPEAT;
			samplerInfo.minLod = 0.0f;
			samplerInfo.maxLod = VK_LOD_CLAMP_NONE;
			samplerInfo.mipLodBias = 0.0f;

			VkSampler sampler = VK_NULL_HANDLE;
			if (const auto res = vkCreateSampler(aWindow.device->device, &samplerInfo, nullptr, &sampler); VK_SUCCESS != res)
				throw Utils::Error("Unable to create sampler\n vkCreateSampler() returned %s", Utils::toString(res).c_str());

			aSamplers.emplace_back(vk::Sampler(aWindow.device->device, sampler));
		}
	}

	void waitForFences(const VulkanWindow& aWindow, std::vector<vk::Fence>& aFences, std::size_t frameIndex) {
		if (const auto res = vkWaitForFences(aWindow.device->device, 1, &aFences[frameIndex].handle, VK_TRUE, std::numeric_limits<std::uint64_t>::max()); VK_SUCCESS != res)
			throw Utils::Error("Unable to wait for frame fence %u\n vkWaitForFences() returned %s", frameIndex, Utils::toString(res).c_str());
	}

	void resetFences(const VulkanWindow& aWindow, std::vector<vk::Fence>& aFences, std::size_t frameIndex) {
		if (const auto res = vkResetFences(aWindow.device->device, 1, &aFences[frameIndex].handle); VK_SUCCESS != res)
			throw Utils::Error("Unable to reset frame fence %u\n vkResetFences() returned %s", frameIndex, Utils::toString(res).c_str());
	}

	void acquireNextSwapchainImage(const VulkanWindow& aWindow, std::vector<vk::Semaphore>& aSemaphores, std::size_t frameIndex, std::uint32_t& imageIndex) {
		const VkResult acquireResult = vkAcquireNextImageKHR(
			aWindow.device->device,
			aWindow.swapchain,
			std::numeric_limits<std::uint64_t>::max(),
			aSemaphores[frameIndex].handle,
			VK_NULL_HANDLE,
			&imageIndex
		);

		if (acquireResult != VK_SUCCESS)
			throw Utils::Error("Unable to acquire next swapchain image\n vkAcquireNextImageKHR() returned %s", Utils::toString(acquireResult).c_str());
	}

}
#pragma once

#include <cstdint>

#include <volk/volk.h>

namespace Utils {

	void imageBarrier(
		VkCommandBuffer aCmdBuff,
		VkImage aImage,
		VkAccessFlags aSrcAccessMask,
		VkAccessFlags aDstAccessMask,
		VkImageLayout aSrcLayout,
		VkImageLayout aDstLayout,
		VkPipelineStageFlags aSrcStageMask,
		VkPipelineStageFlags aDstStageMask,
		VkImageSubresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT,0,1,0,1 },
		std::uint32_t aSrcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		std::uint32_t aDstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED
	);

	void copyBufferToImage(
		VkCommandBuffer aCmdBuff, 
		VkBuffer aBuff, 
		VkImage aImage,
		VkImageSubresourceLayers aLayers = VkImageSubresourceLayers{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
		VkOffset3D aOffset = VkOffset3D{ 0, 0, 0 },
		VkExtent3D aExtent = VkExtent3D{ 1, 1, 1 }
	);

}
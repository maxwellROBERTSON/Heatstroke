#pragma once

#include <cstdint>

#include <volk/volk.h>

namespace Utils {

	void bufferBarrier(
		VkCommandBuffer aCmdBuff,
		VkBuffer aBuff,
		VkAccessFlags aSrcAccessMask,
		VkAccessFlags aDstAccessMask,
		VkPipelineStageFlags aSrcStageMask,
		VkPipelineStageFlags aDstStageMask,
		VkDeviceSize aSize = VK_WHOLE_SIZE,
		VkDeviceSize aOffset = 0,
		uint32_t aSrcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		uint32_t aDstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED
	);

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

	// Returns VK_SAMPLE_COUNT_4_BIT if argument is VK_SAMPLE_COUNT_1_BIT,
	// otherwise just returns the input. Needed since some render passes, attachments,
	// pipelines etc are MSAA specific which require a sample count above 1.
	VkSampleCountFlagBits getMSAAMinimum(VkSampleCountFlagBits sampleCount);
}
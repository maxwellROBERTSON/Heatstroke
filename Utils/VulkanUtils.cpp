#include "VulkanUtils.hpp"
#include "vulkan/vulkan_core.h"
#include <stdexcept>

namespace Utils {

	void bufferBarrier(
		VkCommandBuffer aCmdBuff,
		VkBuffer aBuff,
		VkAccessFlags aSrcAccessMask,
		VkAccessFlags aDstAccessMask,
		VkPipelineStageFlags aSrcStageMask,
		VkPipelineStageFlags aDstStageMask,
		VkDeviceSize aSize,
		VkDeviceSize aOffset,
		uint32_t aSrcQueueFamilyIndex,
		uint32_t aDstQueueFamilyIndex
	) {
		VkBufferMemoryBarrier bbarrier{};
		bbarrier.sType = VK_STRUCTURE_TYPE_BUFFER_MEMORY_BARRIER;
		bbarrier.srcAccessMask = aSrcAccessMask;
		bbarrier.dstAccessMask = aDstAccessMask;
		bbarrier.buffer = aBuff;
		bbarrier.size = aSize;
		bbarrier.offset = aOffset;
		bbarrier.srcQueueFamilyIndex = aSrcQueueFamilyIndex;
		bbarrier.dstQueueFamilyIndex = aDstQueueFamilyIndex;

		vkCmdPipelineBarrier(
			aCmdBuff,
			aSrcStageMask,
			aDstStageMask,
			0,
			0,
			nullptr,
			1,
			&bbarrier,
			0,
			nullptr
		);
	}

	void imageBarrier(
		VkCommandBuffer aCmdBuff,
		VkImage aImage,
		VkAccessFlags aSrcAccessMask,
		VkAccessFlags aDstAccessMask,
		VkImageLayout aSrcLayout,
		VkImageLayout aDstLayout,
		VkPipelineStageFlags aSrcStageMask,
		VkPipelineStageFlags aDstStageMask,
		VkImageSubresourceRange aSubresourceRange,
		std::uint32_t aSrcQueueFamilyIndex,
		std::uint32_t aDstQueueFamilyIndex
	) {
		VkImageMemoryBarrier ibarrier{};
		ibarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
		ibarrier.image = aImage;
		ibarrier.srcAccessMask = aSrcAccessMask;
		ibarrier.dstAccessMask = aDstAccessMask;
		ibarrier.srcQueueFamilyIndex = aSrcQueueFamilyIndex;
		ibarrier.dstQueueFamilyIndex = aDstQueueFamilyIndex;
		ibarrier.oldLayout = aSrcLayout;
		ibarrier.newLayout = aDstLayout;
		ibarrier.subresourceRange = aSubresourceRange;

		vkCmdPipelineBarrier(aCmdBuff, aSrcStageMask, aDstStageMask, 0, 0, nullptr, 0, nullptr, 1, &ibarrier);
	}

	void copyBufferToImage(VkCommandBuffer aCmdBuff, VkBuffer aBuff, VkImage aImage, VkImageSubresourceLayers aLayers, VkOffset3D aOffset, VkExtent3D aExtent) {
		VkBufferImageCopy copy{};
		copy.bufferOffset = 0;
		copy.bufferRowLength = 0;
		copy.bufferImageHeight = 0;
		copy.imageSubresource = aLayers;
		copy.imageOffset = aOffset;
		copy.imageExtent = aExtent;

		vkCmdCopyBufferToImage(aCmdBuff, aBuff, aImage, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &copy);
	}

}
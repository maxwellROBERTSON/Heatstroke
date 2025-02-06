#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include "Texture.hpp"

#include <bit>

#include "Buffer.hpp"
#include "Error.hpp"
#include "toString.hpp"
#include "VulkanUtils.hpp"

namespace Engine {
namespace vk {

	Texture::~Texture() {
		if (image != VK_NULL_HANDLE) {
			assert(mAllocator != VK_NULL_HANDLE);
			assert(allocation != VK_NULL_HANDLE);
			vmaDestroyImage(mAllocator, image, allocation);
		}
	}

	Texture::Texture(VmaAllocator aAllocator, VkImage aImage, VmaAllocation aAllocation) noexcept
		: image(aImage)
		, allocation(aAllocation)
		, mAllocator(aAllocator)
	{}

	Texture::Texture(Texture&& aOther) noexcept
		: image(std::exchange(aOther.image, VK_NULL_HANDLE))
		, allocation(std::exchange(aOther.allocation, VK_NULL_HANDLE))
		, mAllocator(std::exchange(aOther.mAllocator, VK_NULL_HANDLE))
	{}

	Texture& Texture::operator=(Texture&& aOther) noexcept
	{
		std::swap(image, aOther.image);
		std::swap(allocation, aOther.allocation);
		std::swap(mAllocator, aOther.mAllocator);
		return *this;
	}

	Texture createTexture(const VulkanContext& aContext, tinygltf::Image aTinygltfImage, VkFormat aFormat) {
		std::size_t sizeInBytes = aTinygltfImage.image.size();

		vk::Buffer staging = createBuffer(
			aContext,
			sizeInBytes,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		);

		void* stagingPtr = nullptr;
		if (const auto result = vmaMapMemory(aContext.allocator.get()->allocator, staging.allocation, &stagingPtr);  VK_SUCCESS != result)
			throw Utils::Error("Unable to map memory\n vmaMapMemory() returned %s", Utils::toString(result).c_str());

		std::memcpy(stagingPtr, aTinygltfImage.image.data(), sizeInBytes);
		vmaUnmapMemory(aContext.allocator.get()->allocator, staging.allocation);

		std::uint32_t width = aTinygltfImage.width;
		std::uint32_t height = aTinygltfImage.height;

		// Since glTF uses jpeg and png images we need to do mipmapping ourselves
		std::uint32_t mipLevels = computeMipLevels(width, height);

		VkImageUsageFlags usage = VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT;
		Texture texture = createVkImage(aContext, width, height, aFormat, mipLevels, usage);

		VkCommandBuffer cmdBuff = createCommandBuffer(*aContext.window);
		beginCommandBuffer(cmdBuff);

		Utils::imageBarrier(
			cmdBuff,
			texture.image,
			0,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, mipLevels, 0, 1 }
		);

		Utils::copyBufferToImage(
			cmdBuff,
			staging.buffer,
			texture.image,
			VkImageSubresourceLayers{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
			VkOffset3D{ 0, 0, 0 },
			VkExtent3D{ width, height, 1 }
		);

		Utils::imageBarrier(
			cmdBuff,
			texture.image,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 }
		);

		// Do mipmapping
		for (std::uint32_t level = 1; level < mipLevels; level++) {
			VkImageBlit blit{};
			blit.srcSubresource = VkImageSubresourceLayers{ VK_IMAGE_ASPECT_COLOR_BIT, level - 1, 0, 1 };
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { std::int32_t(width), std::int32_t(height), 1 };

			width >>= 1; if (width == 0) width = 1;
			height >>= 1; if (height == 0) height = 1;

			blit.dstSubresource = VkImageSubresourceLayers{ VK_IMAGE_ASPECT_COLOR_BIT, level, 0, 1 };
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { std::int32_t(width), std::int32_t(height), 1 };

			vkCmdBlitImage(
				cmdBuff,
				texture.image,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				texture.image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&blit,
				VK_FILTER_LINEAR
			);

			Utils::imageBarrier(
				cmdBuff,
				texture.image,
				VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_ACCESS_TRANSFER_READ_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, level, 1, 0, 1 }
			);
		}

		Utils::imageBarrier(
			cmdBuff,
			texture.image,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_ACCESS_SHADER_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, mipLevels, 0, 1 }
		);

		endAndSubmitCommandBuffer(*aContext.window, cmdBuff);

		return texture;
	}

	ImageView createImageView(const VulkanWindow& aWindow, VkImage aImage, VkFormat aFormat) {
		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = aImage;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = aFormat;
		viewInfo.components = VkComponentMapping{};
		viewInfo.subresourceRange = VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, VK_REMAINING_MIP_LEVELS, 0, 1 };

		VkImageView view = VK_NULL_HANDLE;
		if (const auto res = vkCreateImageView(aWindow.device->device, &viewInfo, nullptr, &view); VK_SUCCESS != res)
			throw Utils::Error("Unable to create image view\n vkCreateImageView() returned %s", Utils::toString(res).c_str());

		return ImageView(aWindow.device->device, view);
	}

	std::uint32_t computeMipLevels(std::uint32_t width, std::uint32_t height) {
		const std::uint32_t bits = width | height;
		const std::uint32_t leadingZeros = std::countl_zero(bits);
		return 32 - leadingZeros;
	}

	Texture createVkImage(const VulkanContext& aContext, std::uint32_t aWidth, std::uint32_t aHeight, VkFormat aFormat, std::uint32_t aMipLevels, VkImageUsageFlags aFlags) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = aFormat;
		imageInfo.extent.width = aWidth;
		imageInfo.extent.height = aHeight;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = aMipLevels;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = aFlags;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.flags = 0;
		allocInfo.usage = VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE;

		VkImage image = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;

		if (const auto result = vmaCreateImage(aContext.allocator.get()->allocator, &imageInfo, &allocInfo, &image, &allocation, nullptr); VK_SUCCESS != result)
			throw Utils::Error("Unable to allocate image\n vmaCreateImage() returned %s", Utils::toString(result).c_str());

		return Texture(aContext.allocator.get()->allocator, image, allocation);
	}
}
}
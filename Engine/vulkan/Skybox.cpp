#include "Skybox.hpp"

#include "VulkanDevice.hpp"
#include "PipelineCreation.hpp"
#include "VulkanUtils.hpp"
#include "Error.hpp"
#include "toString.hpp"

namespace Engine {

	// Must provide 6 filenames for the 6 sides of the skybox cube
	Skybox::Skybox(VulkanContext* context, std::vector<const char*> filenames) {
		this->context = context;

		this->loadFiles(filenames);

		TextureBufferSetting skyboxSetting = {
			.imageCreateFlags = VK_IMAGE_CREATE_CUBE_COMPATIBLE_BIT,
			.imageFormat = VK_FORMAT_R8G8B8A8_UNORM,
			.imageExtent = VkExtent2D { (std::uint32_t)this->width, (std::uint32_t)this->height },
			.imageArrayLayers = 6,
			.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.viewType = VK_IMAGE_VIEW_TYPE_CUBE,
			.viewAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
			.subresourceLayerCount = 6,
			.ignoreMipLevels = false
		};

		std::pair<vk::Texture, vk::ImageView> texture = createTextureBuffer(*this->context, skyboxSetting);
		this->image = std::move(texture.first);
		this->imageView = std::move(texture.second);

		this->uploadToGPU();

		for (int i = 0; i < 6; i++) {
			free(this->skyboxImageData[i]);
		}

		this->makeDescriptor();
	}

	void Skybox::loadFiles(std::vector<const char*> filenames) {
		// Skybox must only consist of 6 images
		assert(filenames.size() == 6);

		for (int i = 0; i < 6; i++) {
			// Even though width and height will get overwritten as this
			// loop iterates through the files, since each skybox part 
			// should be the same width and height it shouldn't matter
			this->skyboxImageData[i] = stbi_load(filenames[i], &this->width, &this->height, &this->channels, STBI_rgb_alpha);
			if (!this->skyboxImageData[i])
				std::fprintf(stderr, "Unable to load skybox image: %s\n", filenames[i]);
		}
	}

	void Skybox::uploadToGPU() {
		std::size_t imageSize = this->width * this->height * 4;
		std::size_t bufSize = imageSize * 6;
		
		vk::Buffer stagingBuf = vk::createBuffer(
			"skybox",
			*this->context->allocator,
			bufSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		);

		std::uint8_t* ptr = nullptr;
		if (const auto res = vmaMapMemory(this->context->allocator->allocator, stagingBuf.allocation, (void**)&ptr); VK_SUCCESS != res)
			throw Utils::Error("Mapping memory for writing\n vmaMapMemory() returned %s\n", Utils::toString(res).c_str());

		for (int i = 0; i < 6; i++) {
			// Pointer arithmetic to advance pointer so we can copy the next data of next image
			std::memcpy(ptr + (imageSize * i), this->skyboxImageData[i], imageSize);
		}
		
		vmaUnmapMemory(this->context->allocator->allocator, stagingBuf.allocation);

		std::uint32_t mipLevels = computeMipLevels(this->width, this->height);

		VkCommandBuffer cmdBuf = createCommandBuffer(*this->context->window);
		beginCommandBuffer(cmdBuf);

		// Transition to optimal layout
		Utils::imageBarrier(
			cmdBuf,
			this->image.image,
			0,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, mipLevels, 0, 6 }
		);

		Utils::copyBufferToImage(
			cmdBuf,
			stagingBuf.buffer,
			this->image.image,
			VkImageSubresourceLayers{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 6 },
			VkOffset3D{ 0, 0, 0 },
			VkExtent3D{ (std::uint32_t)this->width, (std::uint32_t)this->height, 1 }
		);

		Utils::imageBarrier(
			cmdBuf,
			this->image.image,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 6 }
		);

		std::int32_t mipWidth = this->width;
		std::int32_t mipHeight = this->height;

		// Mipmaps (may not even need to do this for the skybox since it should always be a fixed distance from viewer
		for (std::uint32_t level = 1; level < mipLevels; level++) {
			VkImageBlit blit{};
			blit.srcSubresource = VkImageSubresourceLayers{ VK_IMAGE_ASPECT_COLOR_BIT, level - 1, 0, 1 };
			blit.srcOffsets[0] = { 0, 0, 0 };
			blit.srcOffsets[1] = { mipWidth, mipHeight, 1};

			mipWidth >>= 1; if (mipWidth == 0) mipWidth = 1;
			mipHeight >>= 1; if (mipHeight == 0) mipHeight = 1;

			blit.dstSubresource = VkImageSubresourceLayers{ VK_IMAGE_ASPECT_COLOR_BIT, level, 0, 1 };
			blit.dstOffsets[0] = { 0, 0, 0 };
			blit.dstOffsets[1] = { mipWidth, mipHeight, 1 };

			vkCmdBlitImage(
				cmdBuf,
				this->image.image,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				this->image.image,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				1,
				&blit,
				VK_FILTER_LINEAR
			);

			Utils::imageBarrier(
				cmdBuf,
				this->image.image,
				VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_ACCESS_TRANSFER_READ_BIT,
				VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
				VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, level, 1, 0, 6 }
			);
		}

		Utils::imageBarrier(
			cmdBuf,
			this->image.image,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_ACCESS_SHADER_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, mipLevels, 0, 6 }
		);

		endAndSubmitCommandBuffer(*this->context->window, cmdBuf);

		vk::SamplerInfo samplerInfo;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		this->sampler = createTextureSampler(*this->context->window, samplerInfo);

		this->image.sampler = this->sampler.handle;
	}

	void Skybox::makeDescriptor() {
		std::vector<DescriptorSetting> skyboxDescriptorSetting = { {
			.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER,
			.shaderStageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
		} };

		vk::DescriptorSetLayout descriptorLayout = createDescriptorLayout(*this->context->window, skyboxDescriptorSetting);

		VkDescriptorSet descriptorSet = allocateDescriptorSet(*this->context->window, this->context->window->device->dPool, descriptorLayout.handle);
		{
			VkWriteDescriptorSet desc[1]{};

			VkDescriptorImageInfo skybox{};
			skybox.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			skybox.imageView = this->imageView.handle;
			skybox.sampler = this->sampler.handle;

			desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[0].dstSet = descriptorSet;
			desc[0].dstBinding = 0;
			desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			desc[0].descriptorCount = 1;
			desc[0].pImageInfo = &skybox;

			constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
			vkUpdateDescriptorSets(this->context->window->device->device, numSets, desc, 0, nullptr);
		}

		this->descriptor = descriptorSet;
	}

	void Skybox::bind(VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout) {
		vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, 1, 1, &this->descriptor, 0, nullptr);
	}

}
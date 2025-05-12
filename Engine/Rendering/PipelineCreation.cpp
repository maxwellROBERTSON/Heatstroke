#include "PipelineCreation.hpp"

#include <cassert>
#include <bit>

#include "Error.hpp"
#include "toString.hpp"
#include "Utils.hpp"
#include "../vulkan/VulkanContext.hpp"
#include "../vulkan/VulkanDevice.hpp"
#include "vulkan/vulkan_core.h"

namespace Engine {

	vk::ShaderModule loadShaderModule(const VulkanWindow& aWindow, const char* aSpirvPath) {
		assert(aSpirvPath);

		if (std::FILE* fin = std::fopen(aSpirvPath, "rb")) {
			std::fseek(fin, 0, SEEK_END);
			const auto bytes = std::size_t(std::ftell(fin));
			std::fseek(fin, 0, SEEK_SET);

			assert(0 == bytes % 4);
			const auto words = bytes / 4;

			std::vector<std::uint32_t> code(words);

			std::size_t offset = 0;
			while (offset != words) {
				const auto read = std::fread(code.data() + offset, sizeof(std::uint32_t), words - offset, fin);

				if (0 == read) {
					const auto err = std::ferror(fin), eof = std::feof(fin);
					std::fclose(fin);

					throw Utils::Error("Error reading '%s': ferror: %d, feof = %d", aSpirvPath, err, eof);
				}

				offset += read;
			}

			std::fclose(fin);

			VkShaderModuleCreateInfo moduleInfo{};
			moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleInfo.codeSize = bytes;
			moduleInfo.pCode = code.data();

			VkShaderModule smod = VK_NULL_HANDLE;
			if (const auto res = vkCreateShaderModule(aWindow.device->device, &moduleInfo, nullptr, &smod); VK_SUCCESS != res) {
				throw Utils::Error("Unable to create shader module from %s\n vkShaderCreateShaderModule() returned %s", aSpirvPath, Utils::toString(res).c_str());
			}

			return vk::ShaderModule(aWindow.device->device, smod);
		}

		std::fprintf(stderr, "Cannot open '%s' for reading\n", aSpirvPath);

		throw Utils::Error("Cannot open '%s' for reading", aSpirvPath);
	}

	vk::DescriptorSetLayout createDescriptorLayout(const VulkanWindow& aWindow, std::vector<DescriptorSetting> aDescriptorSettings) {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		
		for (std::size_t i = 0; i < aDescriptorSettings.size(); i++) {
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = (std::uint32_t)i;
			binding.descriptorType = aDescriptorSettings[i].descriptorType;
			binding.descriptorCount = 1;
			binding.stageFlags = aDescriptorSettings[i].shaderStageFlags;

			layoutBindings.emplace_back(binding);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = (std::uint32_t)layoutBindings.size();
		layoutInfo.pBindings = layoutBindings.data();

		VkDescriptorSetLayout layout = VK_NULL_HANDLE;
		if (const auto res = vkCreateDescriptorSetLayout(aWindow.device->device, &layoutInfo, nullptr, &layout); VK_SUCCESS != res)
			throw Utils::Error("Unable to create descriptor set layout\n vkCreateDescriptorSetLayout() returned %s", Utils::toString(res).c_str());

		return vk::DescriptorSetLayout(aWindow.device->device, layout);
	}

	vk::PipelineLayout createPipelineLayout(const VulkanWindow& aWindow, std::vector<VkDescriptorSetLayout>& aDescriptorSetLayouts, std::vector<VkPushConstantRange>& aPushConstantRanges) {
		VkPipelineLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = static_cast<std::uint32_t>(aDescriptorSetLayouts.size());
		layoutInfo.pSetLayouts = aDescriptorSetLayouts.data();
		layoutInfo.pushConstantRangeCount = static_cast<std::uint32_t>(aPushConstantRanges.size());
		layoutInfo.pPushConstantRanges = aPushConstantRanges.data();

		VkPipelineLayout layout = VK_NULL_HANDLE;
		if (const auto res = vkCreatePipelineLayout(aWindow.device->device, &layoutInfo, nullptr, &layout); VK_SUCCESS != res) {
			throw Utils::Error("Unable to create pipeline layout\n vkCreatePipelineLayout() returned %s", Utils::toString(res).c_str());
		}

		return vk::PipelineLayout(aWindow.device->device, layout);
	}

	// Should only be used for render pass attachments
	std::pair<vk::Texture, vk::ImageView> createTextureBuffer(const VulkanContext& aContext, TextureBufferSetting aBufferSetting) {
		std::uint32_t mipLevels = 1;

		if (!aBufferSetting.ignoreMipLevels)
			mipLevels = computeMipLevels(aBufferSetting.imageExtent.width, aBufferSetting.imageExtent.height);

		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.flags = aBufferSetting.imageCreateFlags;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = aBufferSetting.imageFormat;
		imageInfo.extent.width = aBufferSetting.imageExtent.width;
		imageInfo.extent.height = aBufferSetting.imageExtent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = mipLevels;
		imageInfo.arrayLayers = aBufferSetting.imageArrayLayers;
		imageInfo.samples = aBufferSetting.samples;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = aBufferSetting.imageUsage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;
		allocInfo.requiredFlags = aBufferSetting.allocationRequiredFlags;
		allocInfo.preferredFlags = aBufferSetting.allocationPreferredFlags;

		VkImage image = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;

		if (const auto res = vmaCreateImage(aContext.allocator->allocator, &imageInfo, &allocInfo, &image, &allocation, nullptr); VK_SUCCESS != res) {
			std::fprintf(stderr, "Unable to allocate depth buffer image.\n vmaCreateImage() returned %s\n", Utils::toString(res).c_str());
			throw Utils::Error("Unable to allocate depth buffer image.\n vmaCreateImage() returned %s\n", Utils::toString(res).c_str());
		}

		vk::Texture Image(aContext.allocator->allocator, "textureBuffer", image, allocation);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = Image.image;
		viewInfo.viewType = aBufferSetting.viewType;
		viewInfo.format = aBufferSetting.imageFormat;
		viewInfo.components = VkComponentMapping{};
		viewInfo.subresourceRange = VkImageSubresourceRange{ aBufferSetting.viewAspectFlags, 0, 1, 0, aBufferSetting.subresourceLayerCount };

		VkImageView view = VK_NULL_HANDLE;
		if (const auto res = vkCreateImageView(aContext.window->device->device, &viewInfo, nullptr, &view); VK_SUCCESS != res)
			throw Utils::Error("Unable to create image view.\n vkCreateImageView() returned %s", Utils::toString(res).c_str());

		return { std::move(Image), vk::ImageView(aContext.window->device->device, view) };
	}

	std::uint32_t computeMipLevels(std::uint32_t width, std::uint32_t height) {
		const std::uint32_t bits = width | height;
		const std::uint32_t leadingZeros = std::countl_zero(bits);
		return 32 - leadingZeros;
	}

	void createFramebuffers(const VulkanWindow& aWindow, std::vector<vk::Framebuffer>& aFramebuffers, VkRenderPass aRenderPass, std::vector<VkImageView>& aImageViews, VkExtent2D aExtent, bool ignoreSwapchainImage) {
		assert(aFramebuffers.empty());

		for (std::size_t i = 0; i < aWindow.swapViews.size(); ++i) {
			std::vector<VkImageView> attachments;
			if (!ignoreSwapchainImage)
				attachments.push_back(aWindow.swapViews[i]);

			for (std::size_t j = 0; j < aImageViews.size(); j++) {
				attachments.push_back(aImageViews[j]);
			}

			VkFramebufferCreateInfo fbInfo{};
			fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbInfo.flags = 0;
			fbInfo.renderPass = aRenderPass;
			fbInfo.attachmentCount = (std::uint32_t)attachments.size();
			fbInfo.pAttachments = attachments.data();
			fbInfo.width = aExtent.width;
			fbInfo.height = aExtent.height;
			fbInfo.layers = 1;

			VkFramebuffer fb = VK_NULL_HANDLE;
			if (const auto res = vkCreateFramebuffer(aWindow.device->device, &fbInfo, nullptr, &fb); VK_SUCCESS != res)
				throw Utils::Error("Unable to create framebuffer for swap chain image %zu\n vkCreateFramebuffer() returned %s", i, Utils::toString(res).c_str());

			aFramebuffers.emplace_back(vk::Framebuffer(aWindow.device->device, fb));
		}

		assert(aWindow.swapViews.size() == aFramebuffers.size());
	}

	VkDescriptorSet createUBODescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer) {
		VkDescriptorSet uboDescriptor = Engine::allocateDescriptorSet(aWindow, aWindow.device->dPool, aSetLayout);
		{
			VkWriteDescriptorSet desc[1]{};

			VkDescriptorBufferInfo uboInfo{};
			uboInfo.buffer = aBuffer;
			uboInfo.range = VK_WHOLE_SIZE;

			desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[0].dstSet = uboDescriptor;
			desc[0].dstBinding = 0;
			desc[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			desc[0].descriptorCount = 1;
			desc[0].pBufferInfo = &uboInfo;

			constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
			vkUpdateDescriptorSets(aWindow.device->device, numSets, desc, 0, nullptr);
		}

		return uboDescriptor;
	}

	VkDescriptorSet createModelMatricesDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer, VkDeviceSize dynamicAlignment) {
		// Create dynamic UBO for model matrices
		VkDescriptorSet modelMatricesDescriptor = allocateDescriptorSet(aWindow, aWindow.device->dPool, aSetLayout);
		{
			VkWriteDescriptorSet desc[1]{};

			VkDescriptorBufferInfo modelMatricesInfo{};
			modelMatricesInfo.buffer = aBuffer;
			modelMatricesInfo.range = dynamicAlignment;

			desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[0].dstSet = modelMatricesDescriptor;
			desc[0].dstBinding = 0;
			desc[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			desc[0].descriptorCount = 1;
			desc[0].pBufferInfo = &modelMatricesInfo;

			constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
			vkUpdateDescriptorSets(aWindow.device->device, numSets, desc, 0, nullptr);
		}

		return modelMatricesDescriptor;
	}

	VkDescriptorSet createDeferredShadingDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkImageView aDepthView, VkImageView aNormalsView, VkImageView aAlbedoView, VkImageView aEmissiveView) {
		// Create deferred shading descriptor
		VkDescriptorSet deferredShadingDescriptor = allocateDescriptorSet(aWindow, aWindow.device->dPool, aSetLayout);
		{
			VkWriteDescriptorSet desc[4]{};

			VkDescriptorImageInfo inputAttachments[4]{};
			inputAttachments[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			inputAttachments[0].imageView = aNormalsView;
			inputAttachments[0].sampler = VK_NULL_HANDLE;

			inputAttachments[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			inputAttachments[1].imageView = aAlbedoView;
			inputAttachments[1].sampler = VK_NULL_HANDLE;

			inputAttachments[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			inputAttachments[2].imageView = aEmissiveView;
			inputAttachments[2].sampler = VK_NULL_HANDLE;

			inputAttachments[3].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			inputAttachments[3].imageView = aDepthView;
			inputAttachments[3].sampler = VK_NULL_HANDLE;

			desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[0].dstSet = deferredShadingDescriptor;
			desc[0].dstBinding = 0;
			desc[0].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			desc[0].descriptorCount = 1;
			desc[0].pImageInfo = &inputAttachments[0];

			desc[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[1].dstSet = deferredShadingDescriptor;
			desc[1].dstBinding = 1;
			desc[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			desc[1].descriptorCount = 1;
			desc[1].pImageInfo = &inputAttachments[1];

			desc[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[2].dstSet = deferredShadingDescriptor;
			desc[2].dstBinding = 2;
			desc[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			desc[2].descriptorCount = 1;
			desc[2].pImageInfo = &inputAttachments[2];

			desc[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[3].dstSet = deferredShadingDescriptor;
			desc[3].dstBinding = 3;
			desc[3].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			desc[3].descriptorCount = 1;
			desc[3].pImageInfo = &inputAttachments[3];

			constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
			vkUpdateDescriptorSets(aWindow.device->device, numSets, desc, 0, nullptr);
		}

		return deferredShadingDescriptor;
	}

	// Only works for shadow depth image at the moment
	VkDescriptorSet createImageDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkImageLayout aImageLayout, VkImageView aImageView, VkSampler aSampler) {
		VkDescriptorSet imageDescriptor = allocateDescriptorSet(aWindow, aWindow.device->dPool, aSetLayout);
		{
			VkWriteDescriptorSet desc[1]{};

			VkDescriptorImageInfo imageInfo[1]{};
			imageInfo[0].imageLayout = aImageLayout;
			imageInfo[0].imageView = aImageView;
			imageInfo[0].sampler = aSampler;

			desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[0].dstSet = imageDescriptor;
			desc[0].dstBinding = 0;
			desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			desc[0].descriptorCount = 1;
			desc[0].pImageInfo = imageInfo;

			constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
			vkUpdateDescriptorSets(aWindow.device->device, numSets, desc, 0, nullptr);
		}

		return imageDescriptor;
	}
}
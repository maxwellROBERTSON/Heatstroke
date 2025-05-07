#pragma once

#include "VulkanContext.hpp"
#include "objects/VkObjects.hpp"
#include "objects/Texture.hpp"
#include "objects/Buffer.hpp"
#include "Uniforms.hpp"

namespace Engine {
	
	class VulkanWindow;
	class VulkanAllocator;

	struct DescriptorSetting {
		VkDescriptorType descriptorType;
		VkShaderStageFlags shaderStageFlags;
	};

	struct TextureBufferSetting {
		VkImageCreateFlags imageCreateFlags;
		VkFormat imageFormat;
		VkExtent2D imageExtent;
		std::uint32_t imageArrayLayers = 1;
		VkImageUsageFlags imageUsage;
		VkImageViewType viewType = VK_IMAGE_VIEW_TYPE_2D;
		VkImageAspectFlags viewAspectFlags;
		std::uint32_t subresourceLayerCount = 1;
		VkSampleCountFlagBits samples = VK_SAMPLE_COUNT_1_BIT;
		VkMemoryPropertyFlags allocationRequiredFlags = 0;
		VkMemoryPropertyFlags allocationPreferredFlags = 0;
		bool ignoreMipLevels = true;
	};

	vk::ShaderModule loadShaderModule(const VulkanWindow& aWindow, const char* aSpirvPath);

	vk::DescriptorSetLayout createDescriptorLayout(const VulkanWindow& aWindow, std::vector<DescriptorSetting> aDescriptorSettings);

	vk::PipelineLayout createPipelineLayout(const VulkanWindow& aWindow, std::vector<VkDescriptorSetLayout>& layouts, std::vector<VkPushConstantRange>& aPushConstantRanges);

	std::pair<vk::Texture, vk::ImageView> createTextureBuffer(const VulkanContext& aContext, TextureBufferSetting aBufferSetting);
	std::uint32_t computeMipLevels(std::uint32_t width, std::uint32_t height);

	void createFramebuffers(const VulkanWindow& aWindow, std::vector<vk::Framebuffer>& aFramebuffers, VkRenderPass aRenderPass, std::vector<VkImageView>& aImageViews, VkExtent2D aExtent, bool ignoreSwapchainImage = false);

	VkDescriptorSet createUBODescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer);
	VkDescriptorSet createModelMatricesDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer, VkDeviceSize dynamicAlignment);
	VkDescriptorSet createDeferredShadingDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkImageView aDepthView, VkImageView aNormalsView, VkImageView aAlbedoView, VkImageView aEmissiveView);
	VkDescriptorSet createImageDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkImageLayout aImageLayout, VkImageView aImageView, VkSampler aSampler);
}
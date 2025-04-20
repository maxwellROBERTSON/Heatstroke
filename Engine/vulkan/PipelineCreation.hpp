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

	// Default forward render pass. Just swapchain and depth attachments
	vk::RenderPass createRenderPass(const VulkanWindow& aWindow);
	// Default forward render pass with MSAA
	vk::RenderPass createRenderPassMSAA(const VulkanWindow& aWindow, VkSampleCountFlagBits sampleCount);
	// Deferred render pass
	vk::RenderPass createDeferredRenderPass(const VulkanWindow& aWindow);
	// Shadow offscreen render pass
	vk::RenderPass createShadowRenderPass(const VulkanWindow& aWindow);
	// UI render pass
	vk::RenderPass createUIRenderPass(const VulkanWindow& aWindow);
	
	vk::DescriptorSetLayout createDescriptorLayout(const VulkanWindow& aWindow, std::vector<DescriptorSetting> aDescriptorSettings);

	vk::PipelineLayout createPipelineLayout(const VulkanWindow& aWindow, std::vector<VkDescriptorSetLayout>& layouts, bool aNeedPushConstant = false);

	vk::Pipeline createForwardPipeline(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aPipelineLayout, bool shadows, VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT);
	std::tuple<vk::Pipeline, vk::Pipeline> createDeferredPipelines(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aGBufWriteLayout, VkPipelineLayout aShadingLayout);
	vk::Pipeline createShadowOffscreenPipeline(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aPipelineLayout);
	vk::Pipeline createSkyboxPipeline(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aPipelineLayout, VkSampleCountFlagBits msaaSamples = VK_SAMPLE_COUNT_1_BIT);

	std::pair<vk::Texture, vk::ImageView> createTextureBuffer(const VulkanContext& aContext, TextureBufferSetting aBufferSetting);
	std::uint32_t computeMipLevels(std::uint32_t width, std::uint32_t height);

	void createFramebuffers(const VulkanWindow& aWindow, std::vector<vk::Framebuffer>& aFramebuffers, VkRenderPass aRenderPass, std::vector<VkImageView>& aImageViews, VkExtent2D aExtent, bool ignoreSwapchainImage = false);

	VkDescriptorSet createUBODescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer);
	VkDescriptorSet createModelMatricesDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer, VkDeviceSize dynamicAlignment);
	VkDescriptorSet createDeferredShadingDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkImageView aDepthView, VkImageView aNormalsView, VkImageView aAlbedoView, VkImageView aEmissiveView);
	VkDescriptorSet createImageDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkImageView aImageView, VkSampler aSampler);
}
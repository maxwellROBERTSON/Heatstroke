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
		VkFormat imageFormat;
		VkImageUsageFlags imageUsage;
		VkImageAspectFlags imageAspectFlags;
		VkExtent2D extent;
	};

	vk::ShaderModule loadShaderModule(const VulkanWindow& aWindow, const char* aSpirvPath);

	// Default render pass. Just swapchain and depth attachments
	vk::RenderPass createRenderPass(const VulkanWindow& aWindow);
	// Deferred render pass
	vk::RenderPass createDeferredRenderPass(const VulkanWindow& aWindow);
	// Shadow offscreen render pass
	vk::RenderPass createShadowRenderPass(const VulkanWindow& aWindow);
	
	vk::DescriptorSetLayout createDescriptorLayout(const VulkanWindow& aWindow, std::vector<DescriptorSetting> aDescriptorSettings);

	vk::PipelineLayout createPipelineLayout(const VulkanWindow& aWindow, std::vector<VkDescriptorSetLayout>& layouts, bool aNeedPushConstant = false);

	vk::Pipeline createPipeline(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aPipelineLayout);
	std::tuple<vk::Pipeline, vk::Pipeline> createDeferredPipelines(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aGBufWriteLayout, VkPipelineLayout aShadingLayout);
	vk::Pipeline createShadowOffscreenPipeline(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aPipelineLayout);

	std::pair<vk::Texture, vk::ImageView> createTextureBuffer(const VulkanContext& aContext, TextureBufferSetting aBufferSetting);

	void createFramebuffers(const VulkanWindow& aWindow, std::vector<vk::Framebuffer>& aFramebuffers, VkRenderPass aRenderPass, std::vector<VkImageView>& aImageViews, VkExtent2D aExtent, bool ignoreSwapchainImage = false);

	VkDescriptorSet createUBODescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer);
	VkDescriptorSet createModelMatricesDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer, VkDeviceSize dynamicAlignment);
	VkDescriptorSet createDeferredShadingDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkImageView aDepthView, VkImageView aNormalsView, VkImageView aAlbedoView, VkImageView aEmissiveView);
	VkDescriptorSet createImageDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkImageView aImageView, VkSampler aSampler);
}
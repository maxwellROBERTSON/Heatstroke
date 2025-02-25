#pragma once

#include "objects/VkObjects.hpp"
#include "objects/Texture.hpp"

namespace Engine {
	
	class VulkanWindow;
	class VulkanAllocator;

	vk::ShaderModule loadShaderModule(const VulkanWindow& aWindow, const char* aSpirvPath);

	// Default render pass. Just swapchain and depth attachments
	vk::RenderPass createRenderPass(const VulkanWindow& aWindow);
	
	// Descriptor layout for projection, view, and model matrices
	vk::DescriptorSetLayout createSceneLayout(const VulkanWindow& aWindow);
	// Descriptor layout for materials
	vk::DescriptorSetLayout createMaterialLayout(const VulkanWindow& aWindow);
	// General descriptor layout for UBO's
	// (might be able to get rid of scene layout and just use this method, but
	// need to double check just substituting different stage flags is fine)
	vk::DescriptorSetLayout createUBOLayout(const VulkanWindow& aWindow);
	vk::DescriptorSetLayout createSSBOLayout(const VulkanWindow& aWindow);

	vk::PipelineLayout createPipelineLayout(const VulkanWindow& aWindow, std::vector<VkDescriptorSetLayout>& layouts, bool aNeedPushConstant = false);

	vk::Pipeline createPipeline(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aPipelineLayout);

	std::tuple<vk::Texture, vk::ImageView> createDepthBuffer(const VulkanWindow& aWindow, const VulkanAllocator& aAllocator);

	void createFramebuffers(const VulkanWindow& aWindow, std::vector<vk::Framebuffer>& aFramebuffers, VkRenderPass aRenderPass, VkImageView aDepthView);

	VkDescriptorSet createSceneDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer);
	VkDescriptorSet createMaterialInfoDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer);

}
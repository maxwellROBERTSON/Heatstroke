#pragma once

#include "VulkanContext.hpp"
#include "objects/VkObjects.hpp"
#include "objects/Texture.hpp"
#include "objects/Buffer.hpp"
#include "Uniforms.hpp"

namespace Engine {
	
	class VulkanWindow;
	class VulkanAllocator;

	vk::ShaderModule loadShaderModule(const VulkanWindow& aWindow, const char* aSpirvPath);

	// Default render pass. Just swapchain and depth attachments
	vk::RenderPass createRenderPass(const VulkanWindow& aWindow);
	// Deferred render pass
	vk::RenderPass createDeferredRenderPass(const VulkanWindow& aWindow);
	
	// Descriptor layout for projection, view, and model matrices
	vk::DescriptorSetLayout createSceneLayout(const VulkanWindow& aWindow);
	// Descriptor layout for materials
	vk::DescriptorSetLayout createMaterialLayout(const VulkanWindow& aWindow);
	// General descriptor layout for UBO's
	// (might be able to get rid of scene layout and just use this method, but
	// need to double check just substituting different stage flags is fine)
	vk::DescriptorSetLayout createUBOLayout(const VulkanWindow& aWindow);
	vk::DescriptorSetLayout createSSBOLayout(const VulkanWindow& aWindow);
	vk::DescriptorSetLayout createDynamicUBOLayout(const VulkanWindow& aWindow);
	vk::DescriptorSetLayout createDeferredLayout(const VulkanWindow& aWindow);

	vk::PipelineLayout createPipelineLayout(const VulkanWindow& aWindow, std::vector<VkDescriptorSetLayout>& layouts, bool aNeedPushConstant = false);

	vk::Pipeline createPipeline(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aPipelineLayout);
	std::tuple<vk::Pipeline, vk::Pipeline> createDeferredPipelines(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aGBufWriteLayout, VkPipelineLayout aShadingLayout);

	// All the buffer methods could be put into one, just pass the image and view format, image usage flags, and view subresource range
	std::tuple<vk::Texture, vk::ImageView> createDepthBuffer(const VulkanWindow& aWindow, const VulkanAllocator& aAllocator);
	std::tuple<vk::Texture, vk::ImageView> createNormalsBuffer(const VulkanWindow& aWindow, const VulkanAllocator& aAllocator);
	std::tuple<vk::Texture, vk::ImageView> createAlbedoBuffer(const VulkanWindow& aWindow, const VulkanAllocator& aAllocator);
	std::tuple<vk::Texture, vk::ImageView> createEmissiveBuffer(const VulkanWindow& aWindow, const VulkanAllocator& aAllocator);

	// Again these methods could be put into one, just pass vector of image views
	void createFramebuffers(const VulkanWindow& aWindow, std::vector<vk::Framebuffer>& aFramebuffers, VkRenderPass aRenderPass, VkImageView aDepthView);
	void createDeferredFramebuffers(const VulkanWindow& aWindow, std::vector<vk::Framebuffer>& aFramebuffers, VkRenderPass aRenderPass, VkImageView aDepthView, VkImageView aNormalsView, VkImageView aAlbedoView, VkImageView aEmissiveView);

	VkDescriptorSet createUBODescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer);
	VkDescriptorSet createModelMatricesDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer, VkDeviceSize dynamicAlignment);
	VkDescriptorSet createDeferredShadingDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkImageView aDepthView, VkImageView aNormalsView, VkImageView aAlbedoView, VkImageView aEmissiveView);

	vk::Buffer setupDynamicUBO(const VulkanContext& aContext, std::size_t modelSize, std::size_t dynamicAlignment, glsl::ModelMatricesUniform& aModelMatrices);

}
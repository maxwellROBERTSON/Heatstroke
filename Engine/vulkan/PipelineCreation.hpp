#pragma once

#include "objects/VkObjects.hpp"
#include "objects/Texture.hpp"
#include "VulkanWindow.hpp"

namespace Engine {

	vk::ShaderModule loadShaderModule(const VulkanWindow& aWindow, const char* aSpirvPath);

	// Default render pass. Just swapchain and depth attachments
	vk::RenderPass createRenderPass(const VulkanWindow& aWindow);
	
	// Descriptor layout for projection, view, and model matrices
	vk::DescriptorSetLayout createSceneLayout(const VulkanWindow& aWindow);
	// Descriptor layout for materials
	vk::DescriptorSetLayout createMaterialLayout(const VulkanWindow& aWindow);

	vk::PipelineLayout createPipelineLayout(const VulkanWindow& aWindow, std::vector<VkDescriptorSetLayout>& layouts);

	vk::Pipeline createPipeline(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aPipelineLayout);

	std::tuple<vk::Texture, vk::ImageView> createDepthBuffer(const VulkanWindow& aWindow, const VulkanAllocator& aAllocator);

	void createFramebuffers(const VulkanWindow& aWindow, std::vector<vk::Framebuffer>& aFramebuffers, VkRenderPass aRenderPass, VkImageView aDepthView);

}
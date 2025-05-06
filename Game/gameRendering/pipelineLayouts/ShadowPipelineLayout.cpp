#include "ShadowPipelineLayout.hpp"

#include <vector>

#include "Engine/vulkan/PipelineCreation.hpp"

ShadowPipelineLayout::ShadowPipelineLayout(
	Engine::VulkanWindow* window,
	std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts) : HsPipelineLayout(window) {
	this->descriptorLayouts = descriptorLayouts;

	this->recreate();
}

void ShadowPipelineLayout::recreate() {
	std::vector<VkDescriptorSetLayout> shadowLayout;
	shadowLayout.emplace_back(this->descriptorLayouts->at("vertUBOLayout").handle); // Depth matrix
	shadowLayout.emplace_back(this->descriptorLayouts->at("vertUBOLayout").handle); // Joint matrices

	VkPushConstantRange modelMatrixPushConstant = {
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.size = sizeof(glm::mat4)
	};

	std::vector<VkPushConstantRange> justModelMatrix;
	justModelMatrix.emplace_back(modelMatrixPushConstant);

	this->pipelineLayout = Engine::createPipelineLayout(*this->window, shadowLayout, justModelMatrix);
}
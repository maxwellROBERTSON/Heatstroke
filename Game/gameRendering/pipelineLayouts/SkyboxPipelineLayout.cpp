#include "SkyboxPipelineLayout.hpp"

#include <vector>

#include "Engine/vulkan/PipelineCreation.hpp"

SkyboxPipelineLayout::SkyboxPipelineLayout(
	Engine::VulkanWindow* window,
	std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts) : HsPipelineLayout(window) {
	this->descriptorLayouts = descriptorLayouts;

	this->recreate();
}

void SkyboxPipelineLayout::recreate() {
	std::vector<VkDescriptorSetLayout> skyboxLayout;
	skyboxLayout.emplace_back(this->descriptorLayouts->at("sceneLayout").handle);     // Projection matrices
	skyboxLayout.emplace_back(this->descriptorLayouts->at("fragImageLayout").handle); // Skybox texture

	std::vector<VkPushConstantRange> emptyPushConstant;

	this->pipelineLayout = Engine::createPipelineLayout(*this->window, skyboxLayout, emptyPushConstant);
}
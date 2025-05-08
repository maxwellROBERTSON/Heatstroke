#include "DecalPipelineLayout.hpp"

#include <vector>

#include "../../../PipelineCreation.hpp"

DecalPipelineLayout::DecalPipelineLayout(
	Engine::VulkanWindow* window,
	std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts) : HsPipelineLayout(window) {
	this->descriptorLayouts = descriptorLayouts;

	this->recreate();
}

void DecalPipelineLayout::recreate() {
	std::vector<VkDescriptorSetLayout> decalLayout;
	decalLayout.emplace_back(this->descriptorLayouts->at("sceneLayout").handle);	 // Projection matrices
	decalLayout.emplace_back(this->descriptorLayouts->at("vertUBOLayout").handle);	 // Transforms buffer
	decalLayout.emplace_back(this->descriptorLayouts->at("fragImageLayout").handle); // Decal texture

	std::vector<VkPushConstantRange> emptyPushConstant;

	this->pipelineLayout = Engine::createPipelineLayout(*this->window, decalLayout, emptyPushConstant);
}
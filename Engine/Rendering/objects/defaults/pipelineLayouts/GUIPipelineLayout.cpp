#include "GUIPipelineLayout.hpp"

#include <vector>

#include "../../../PipelineCreation.hpp"

GUIPipelineLayout::GUIPipelineLayout(
	Engine::VulkanWindow* window,
	std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts) : HsPipelineLayout(window) {
	this->descriptorLayouts = descriptorLayouts;

	this->recreate();
}

void GUIPipelineLayout::recreate() {
	std::vector<VkDescriptorSetLayout> guiLayout;
	guiLayout.emplace_back(this->descriptorLayouts->at("orthoMatrices").handle); // Projection matrices

	std::vector<VkPushConstantRange> emptyPushConstant;

	this->pipelineLayout = Engine::createPipelineLayout(*this->window, guiLayout, emptyPushConstant);
}
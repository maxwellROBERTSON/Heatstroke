#include "ForwardPipelineLayout.hpp"

#include <vector>

#include "Engine/vulkan/PipelineCreation.hpp"

#include <glm/mat4x4.hpp>

ForwardPipelineLayout::ForwardPipelineLayout(
	Engine::VulkanWindow* window,
	std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts,
	bool* shadowsEnabled) : HsPipelineLayout(window) {
	this->descriptorLayouts = descriptorLayouts;

	this->shadowsEnabled = shadowsEnabled;

	this->recreate();
}

void ForwardPipelineLayout::recreate() {
	std::vector<VkDescriptorSetLayout> forwardLayouts;
	forwardLayouts.emplace_back(this->descriptorLayouts->at("sceneLayout").handle);    // Projection 
	forwardLayouts.emplace_back(this->descriptorLayouts->at("vertUBOLayout").handle);  // Joint matrices
	forwardLayouts.emplace_back(this->descriptorLayouts->at("materialLayout").handle); // Materials
	forwardLayouts.emplace_back(this->descriptorLayouts->at("fragSSBOLayout").handle); // Material infos

	std::vector<VkDescriptorSetLayout> forwardShadowLayouts;
	forwardShadowLayouts.emplace_back(this->descriptorLayouts->at("sceneLayout").handle);     // Projection matrices
	forwardShadowLayouts.emplace_back(this->descriptorLayouts->at("vertUBOLayout").handle);   // Joint matrices
	forwardShadowLayouts.emplace_back(this->descriptorLayouts->at("materialLayout").handle);  // Materials
	forwardShadowLayouts.emplace_back(this->descriptorLayouts->at("fragSSBOLayout").handle);  // Material infos
	forwardShadowLayouts.emplace_back(this->descriptorLayouts->at("vertUBOLayout").handle);   // Shadow matrix
	forwardShadowLayouts.emplace_back(this->descriptorLayouts->at("fragImageLayout").handle); // Shadow map

	VkPushConstantRange modelMatrixPushConstant = {
		.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
		.size = sizeof(glm::mat4) };

	VkPushConstantRange materialIndexPushConstant = {
		.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
		.offset = sizeof(glm::mat4),
		.size = sizeof(std::uint32_t) };

	std::vector<VkPushConstantRange> modelMatrixAndMatIdxPushConstant;
	modelMatrixAndMatIdxPushConstant.emplace_back(modelMatrixPushConstant);
	modelMatrixAndMatIdxPushConstant.emplace_back(materialIndexPushConstant);

	this->pipelineLayout = Engine::createPipelineLayout(*this->window, *this->shadowsEnabled ? forwardShadowLayouts : forwardLayouts, modelMatrixAndMatIdxPushConstant);
}
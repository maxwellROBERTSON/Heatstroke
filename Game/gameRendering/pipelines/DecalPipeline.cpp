#include "DecalPipeline.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "Engine/vulkan/VulkanDevice.hpp"
#include "Engine/vulkan/PipelineCreation.hpp"
#include "Engine/Rendering/HsRenderPass.hpp"

DecalPipeline::DecalPipeline(
	Engine::VulkanWindow* window,
	PipelineLayout* pipelineLayout,
	Engine::HsRenderPass* renderPass,
	VkSampleCountFlagBits* sampleCount) : HsPipeline(window) {
	this->sampleCount = sampleCount;

	this->pipelineLayout = pipelineLayout;
	this->renderPass = &renderPass->getRenderPass();

	this->recreate();
}

void DecalPipeline::recreate() {
	Engine::vk::ShaderModule vert = Engine::loadShaderModule(*this->window, "Engine/Shaders/spv/decal.vert.spv");
	Engine::vk::ShaderModule frag = Engine::loadShaderModule(*this->window, "Engine/Shaders/spv/decal.frag.spv");

	VkPipelineShaderStageCreateInfo stages[2]{};
	stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	stages[0].module = vert.handle;
	stages[0].pName = "main";

	stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stages[1].module = frag.handle;
	stages[1].pName = "main";

	VkVertexInputBindingDescription vertexInputs[1]{};
	vertexInputs[0].binding = 0;
	vertexInputs[0].stride = sizeof(float) * 3;
	vertexInputs[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription vertexAttributes[1]{};
	vertexAttributes[0].binding = 0;
	vertexAttributes[0].location = 0;
	vertexAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexAttributes[0].offset = 0;

	VkPipelineVertexInputStateCreateInfo inputInfo{};
	inputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	inputInfo.vertexBindingDescriptionCount = 1;
	inputInfo.pVertexBindingDescriptions = vertexInputs;
	inputInfo.vertexAttributeDescriptionCount = 1;
	inputInfo.pVertexAttributeDescriptions = vertexAttributes;

	VkPipelineInputAssemblyStateCreateInfo assemblyInfo{};
	assemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assemblyInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = float(this->window->swapchainExtent.width);
	viewport.height = float(this->window->swapchainExtent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = VkOffset2D{ 0, 0 };
	scissor.extent = this->window->swapchainExtent;

	VkPipelineViewportStateCreateInfo viewportInfo{};
	viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportInfo.viewportCount = 1;
	viewportInfo.pViewports = &viewport;
	viewportInfo.scissorCount = 1;
	viewportInfo.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterInfo{};
	rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterInfo.depthClampEnable = VK_FALSE;
	rasterInfo.rasterizerDiscardEnable = VK_FALSE;
	rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
	rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterInfo.depthBiasEnable = VK_FALSE;
	rasterInfo.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo samplingInfo{};
	samplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	samplingInfo.rasterizationSamples = *this->sampleCount;

	VkPipelineColorBlendAttachmentState blendStates[1]{};
	blendStates[0].blendEnable = VK_TRUE;
	blendStates[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	blendStates[0].srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	blendStates[0].dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	blendStates[0].colorBlendOp = VK_BLEND_OP_ADD;
	blendStates[0].srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	blendStates[0].dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	blendStates[0].alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo blendInfo{};
	blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendInfo.logicOpEnable = VK_FALSE;
	blendInfo.attachmentCount = 1;
	blendInfo.pAttachments = blendStates;

	VkPipelineDepthStencilStateCreateInfo depthInfo{};
	depthInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthInfo.depthTestEnable = VK_TRUE;
	depthInfo.depthWriteEnable = VK_TRUE;
	depthInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthInfo.minDepthBounds = 0.0f;
	depthInfo.maxDepthBounds = 1.0f;

	VkGraphicsPipelineCreateInfo pipeInfo{};
	pipeInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeInfo.stageCount = 2;
	pipeInfo.pStages = stages;
	pipeInfo.pVertexInputState = &inputInfo;
	pipeInfo.pInputAssemblyState = &assemblyInfo;
	pipeInfo.pTessellationState = nullptr;
	pipeInfo.pViewportState = &viewportInfo;
	pipeInfo.pRasterizationState = &rasterInfo;
	pipeInfo.pMultisampleState = &samplingInfo;
	pipeInfo.pDepthStencilState = &depthInfo;
	pipeInfo.pColorBlendState = &blendInfo;
	pipeInfo.pDynamicState = nullptr;
	pipeInfo.layout = this->pipelineLayout->get()->getPipelineLayoutHandle();
	pipeInfo.renderPass = this->renderPass->handle;
	pipeInfo.subpass = 0;

	VkPipeline pipe = VK_NULL_HANDLE;
	if (const auto res = vkCreateGraphicsPipelines(this->window->device->device, VK_NULL_HANDLE, 1, &pipeInfo, nullptr, &pipe); VK_SUCCESS != res) {
		throw Utils::Error("Unable to create graphics pipeline\n vkCreateGraphicsPipeline() returned %s", Utils::toString(res).c_str());
	}

	this->pipeline = Engine::vk::Pipeline(this->window->device->device, pipe);
}
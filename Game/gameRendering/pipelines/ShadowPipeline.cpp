#include "ShadowPipeline.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "Engine/vulkan/VulkanDevice.hpp"
#include "Engine/vulkan/PipelineCreation.hpp"
#include "Engine/Rendering/HsRenderPass.hpp"

ShadowPipeline::ShadowPipeline(
	Engine::VulkanWindow* window,
	PipelineLayout* pipelineLayout,
	Engine::HsRenderPass* renderPass,
	VkSampleCountFlagBits* sampleCount,
	VkExtent2D* shadowMapResolution) : HsPipeline(window) {
	this->sampleCount = sampleCount;

	this->pipelineLayout = pipelineLayout;
	this->renderPass = &renderPass->getRenderPass();

	this->shadowMapResolution = shadowMapResolution;

	this->recreate();
}

void ShadowPipeline::recreate() {
	Engine::vk::ShaderModule vert = Engine::loadShaderModule(*this->window, "Engine/Shaders/spv/shadowOffscreen.vert.spv");

	VkPipelineShaderStageCreateInfo stages[1]{};
	stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	stages[0].module = vert.handle;
	stages[0].pName = "main";

	// 1. Position
	// 2. Joints
	// 3. Joint weights
	VkVertexInputBindingDescription vertexInputs[3]{};
	vertexInputs[0].binding = 0;
	vertexInputs[0].stride = sizeof(float) * 3;
	vertexInputs[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInputs[1].binding = 1;
	vertexInputs[1].stride = sizeof(unsigned int) * 4;
	vertexInputs[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInputs[2].binding = 2;
	vertexInputs[2].stride = sizeof(float) * 4;
	vertexInputs[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription vertexAttributes[3]{};
	vertexAttributes[0].binding = 0;
	vertexAttributes[0].location = 0;
	vertexAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexAttributes[0].offset = 0;
	vertexAttributes[1].binding = 1;
	vertexAttributes[1].location = 1;
	vertexAttributes[1].format = VK_FORMAT_R32G32B32A32_UINT;
	vertexAttributes[1].offset = 0;
	vertexAttributes[2].binding = 2;
	vertexAttributes[2].location = 2;
	vertexAttributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	vertexAttributes[2].offset = 0;

	VkPipelineVertexInputStateCreateInfo inputInfo{};
	inputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	inputInfo.vertexBindingDescriptionCount = 3;
	inputInfo.pVertexBindingDescriptions = vertexInputs;
	inputInfo.vertexAttributeDescriptionCount = 3;
	inputInfo.pVertexAttributeDescriptions = vertexAttributes;

	VkPipelineInputAssemblyStateCreateInfo assemblyInfo{};
	assemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	assemblyInfo.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport{};
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(this->shadowMapResolution->width);
	viewport.height = static_cast<float>(this->shadowMapResolution->height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor{};
	scissor.offset = VkOffset2D{ 0, 0 };
	scissor.extent = *this->shadowMapResolution;

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
	rasterInfo.cullMode = VK_CULL_MODE_NONE;
	rasterInfo.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterInfo.depthBiasEnable = VK_TRUE;
	rasterInfo.lineWidth = 1.0f;

	VkPipelineMultisampleStateCreateInfo samplingInfo{};
	samplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	samplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState blendStates[1]{};
	blendStates[0].blendEnable = VK_FALSE;
	blendStates[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

	VkPipelineColorBlendStateCreateInfo blendInfo{};
	blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	blendInfo.logicOpEnable = VK_FALSE;
	blendInfo.attachmentCount = 0;
	blendInfo.pAttachments = blendStates;

	VkPipelineDepthStencilStateCreateInfo depthInfo{};
	depthInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthInfo.depthTestEnable = VK_TRUE;
	depthInfo.depthWriteEnable = VK_TRUE;
	depthInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthInfo.minDepthBounds = 0.0f;
	depthInfo.maxDepthBounds = 1.0f;

	VkDynamicState dynamicStates{
		VK_DYNAMIC_STATE_DEPTH_BIAS
	};

	VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
	dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicStateInfo.dynamicStateCount = 1;
	dynamicStateInfo.pDynamicStates = &dynamicStates;

	VkGraphicsPipelineCreateInfo pipeInfo{};
	pipeInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipeInfo.stageCount = 1;
	pipeInfo.pStages = stages;
	pipeInfo.pVertexInputState = &inputInfo;
	pipeInfo.pInputAssemblyState = &assemblyInfo;
	pipeInfo.pTessellationState = nullptr;
	pipeInfo.pViewportState = &viewportInfo;
	pipeInfo.pRasterizationState = &rasterInfo;
	pipeInfo.pMultisampleState = &samplingInfo;
	pipeInfo.pDepthStencilState = &depthInfo;
	pipeInfo.pColorBlendState = &blendInfo;
	pipeInfo.pDynamicState = &dynamicStateInfo;
	pipeInfo.layout = this->pipelineLayout->get()->getPipelineLayoutHandle();
	pipeInfo.renderPass = this->renderPass->handle;
	pipeInfo.subpass = 0;

	VkPipeline pipe = VK_NULL_HANDLE;
	if (const auto res = vkCreateGraphicsPipelines(this->window->device->device, VK_NULL_HANDLE, 1, &pipeInfo, nullptr, &pipe); VK_SUCCESS != res) {
		throw Utils::Error("Unable to create graphics pipeline\n vkCreateGraphicsPipeline() returned %s", Utils::toString(res).c_str());
	}

	this->pipeline = Engine::vk::Pipeline(this->window->device->device, pipe);
}
#include "ForwardPipeline.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "Engine/vulkan/VulkanDevice.hpp"
#include "Engine/vulkan/PipelineCreation.hpp"
#include "Engine/Rendering/HsRenderPass.hpp"

ForwardPipeline::ForwardPipeline(
	Engine::VulkanWindow* window,
	PipelineLayout* pipelineLayout,
	Engine::HsRenderPass* renderPass,
	VkSampleCountFlagBits* sampleCount,
	bool* shadows) : HsPipeline(window) {
	this->sampleCount = sampleCount;

	this->pipelineLayout = pipelineLayout;
	this->renderPass = &renderPass->getRenderPass();

	this->shadows = shadows;

	this->recreate();
}

void ForwardPipeline::recreate() {
	Engine::vk::ShaderModule vert = Engine::loadShaderModule(*this->window, "Engine/Shaders/spv/forward.vert.spv");
	Engine::vk::ShaderModule frag = Engine::loadShaderModule(*this->window, "Engine/Shaders/spv/forward.frag.spv");

	if (*this->shadows) {
		vert = Engine::loadShaderModule(*this->window, "Engine/Shaders/spv/forwardShadow.vert.spv");
		frag = Engine::loadShaderModule(*this->window, "Engine/Shaders/spv/forwardShadow.frag.spv");
	}

	VkPipelineShaderStageCreateInfo stages[2]{};
	stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
	stages[0].module = vert.handle;
	stages[0].pName = "main";

	stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
	stages[1].module = frag.handle;
	stages[1].pName = "main";

	// Vertex attributes:
	// 1. Positions
	// 2. Normals
	// 3. Tangents
	// 4. Texture coordinates 0
	// 5. Texture coordinates 1 (in glTF some textures refer to a different set of texture coordinates
	//	  so we have to account for multiple sets of texture coordinates. In theory any number
	//	  of texture coordinates could be defined, but in practice with our current models only
	//    one other texture coordinate set is referred to)
	// 6. Vertex colours (vertex colours is a valid glTF vertex attribute but will most likely not exist
	//    at the same time as texture coordinates, so a optimisation would be to dynamically create render passes
	//    based on only the attributes that exist in the glTF file, so that we are not sending both at the same time
	//    as one will just be default values)
	// 7. Joint indexs for vertex skinning
	// 8. Weights for vertex skinning
	VkVertexInputBindingDescription vertexInputs[8]{};
	vertexInputs[0].binding = 0;
	vertexInputs[0].stride = sizeof(float) * 3;
	vertexInputs[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInputs[1].binding = 1;
	vertexInputs[1].stride = sizeof(float) * 3;
	vertexInputs[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInputs[2].binding = 2;
	vertexInputs[2].stride = sizeof(float) * 4;
	vertexInputs[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInputs[3].binding = 3;
	vertexInputs[3].stride = sizeof(float) * 2;
	vertexInputs[3].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInputs[4].binding = 4;
	vertexInputs[4].stride = sizeof(float) * 2;
	vertexInputs[4].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInputs[5].binding = 5;
	vertexInputs[5].stride = sizeof(float) * 4;
	vertexInputs[5].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInputs[6].binding = 6;
	vertexInputs[6].stride = sizeof(unsigned int) * 4;
	vertexInputs[6].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
	vertexInputs[7].binding = 7;
	vertexInputs[7].stride = sizeof(float) * 4;
	vertexInputs[7].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

	VkVertexInputAttributeDescription vertexAttributes[8]{};
	vertexAttributes[0].binding = 0;
	vertexAttributes[0].location = 0;
	vertexAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexAttributes[0].offset = 0;
	vertexAttributes[1].binding = 1;
	vertexAttributes[1].location = 1;
	vertexAttributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	vertexAttributes[1].offset = 0;
	vertexAttributes[2].binding = 2;
	vertexAttributes[2].location = 2;
	vertexAttributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	vertexAttributes[2].offset = 0;
	vertexAttributes[3].binding = 3;
	vertexAttributes[3].location = 3;
	vertexAttributes[3].format = VK_FORMAT_R32G32_SFLOAT;
	vertexAttributes[3].offset = 0;
	vertexAttributes[4].binding = 4;
	vertexAttributes[4].location = 4;
	vertexAttributes[4].format = VK_FORMAT_R32G32_SFLOAT;
	vertexAttributes[4].offset = 0;
	vertexAttributes[5].binding = 5;
	vertexAttributes[5].location = 5;
	vertexAttributes[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	vertexAttributes[5].offset = 0;
	vertexAttributes[6].binding = 6;
	vertexAttributes[6].location = 6;
	vertexAttributes[6].format = VK_FORMAT_R32G32B32A32_UINT;
	vertexAttributes[6].offset = 0;
	vertexAttributes[7].binding = 7;
	vertexAttributes[7].location = 7;
	vertexAttributes[7].format = VK_FORMAT_R32G32B32A32_SFLOAT;
	vertexAttributes[7].offset = 0;

	VkPipelineVertexInputStateCreateInfo inputInfo{};
	inputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	inputInfo.vertexBindingDescriptionCount = 8;
	inputInfo.pVertexBindingDescriptions = vertexInputs;
	inputInfo.vertexAttributeDescriptionCount = 8;
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
	samplingInfo.rasterizationSamples = *sampleCount;

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

	VkDynamicState dynamicStates[1] = {
		VK_DYNAMIC_STATE_CULL_MODE
	};

	VkPipelineDynamicStateCreateInfo dynamicInfo{};
	dynamicInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
	dynamicInfo.dynamicStateCount = 1;
	dynamicInfo.pDynamicStates = dynamicStates;

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
	pipeInfo.pDynamicState = &dynamicInfo;
	pipeInfo.layout = this->pipelineLayout->get()->getPipelineLayoutHandle();
	pipeInfo.renderPass = this->renderPass->handle;
	pipeInfo.subpass = 0;

	VkPipeline pipe = VK_NULL_HANDLE;
	if (const auto res = vkCreateGraphicsPipelines(this->window->device->device, VK_NULL_HANDLE, 1, &pipeInfo, nullptr, &pipe); VK_SUCCESS != res) {
		throw Utils::Error("Unable to create graphics pipeline\n vkCreateGraphicsPipeline() returned %s", Utils::toString(res).c_str());
	}

	this->pipeline = Engine::vk::Pipeline(this->window->device->device, pipe);
}
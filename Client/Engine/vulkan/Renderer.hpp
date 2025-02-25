#pragma once

#include "objects/Model.hpp"
#include "Uniforms.hpp"

namespace Engine {

	void renderModel(
		vk::Model& model, 
		VkCommandBuffer aCmdBuf,
		VkRenderPass aRenderPass,
		VkFramebuffer aFramebuffer,
		VkPipeline aPipeline,
		const VkExtent2D& aExtent,
		VkBuffer aSceneUBO,
		glsl::SceneUniform aSceneUniform,
		VkPipelineLayout aPipelineLayout,
		VkDescriptorSet aSceneDescriptorSet
	);

}
#pragma once

#include "objects/Model.hpp"
#include "Uniforms.hpp"
#include "../ECS/EntityManager.hpp"

namespace Engine {

	void renderModels(
		EntityManager& entityManager,
		std::vector<vk::Model>& models, 
		VkCommandBuffer aCmdBuf,
		VkRenderPass aRenderPass,
		VkFramebuffer aFramebuffer,
		VkPipeline aPipeline,
		const VkExtent2D& aExtent,
		VkBuffer aSceneUBO,
		glsl::SceneUniform aSceneUniform,
		VkPipelineLayout aPipelineLayout,
		VkDescriptorSet aSceneDescriptorSet,
		VkDescriptorSet modelMatricesDescriptor,
		std::uint32_t dynamicOffset
	);

	void renderModelsDeferred(
		EntityManager& entityManager,
		std::vector<vk::Model>& models,
		VkCommandBuffer aCmdBuf,
		VkRenderPass aRenderPass,
		VkFramebuffer aFramebuffer,
		VkPipeline aGBufPipeline,
		VkPipeline aDeferredPipeline,
		const VkExtent2D& aExtent,
		VkBuffer aSceneUBO,
		VkBuffer aLightsUBO,
		glsl::SceneUniform aSceneUniform,
		glsl::LightsUniform aLightsUniform,
		VkPipelineLayout aGBufPipelineLayout,
		VkPipelineLayout aDeferredPipelineLayout,
		VkDescriptorSet aSceneDescriptorSet,
		VkDescriptorSet modelMatricesDescriptor,
		VkDescriptorSet aDeferredShadingDescriptor,
		VkDescriptorSet aLightsDescriptor,
		std::uint32_t dynamicOffset
	);

}
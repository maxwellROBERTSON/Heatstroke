#include "Renderer.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "VulkanUtils.hpp"
#include "VulkanDevice.hpp"

#include "../ECS/RenderComponent.hpp"

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
	) {
		// Begin recording
		beginCommandBuffer(aCmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		Utils::bufferBarrier(
			aCmdBuf,
			aSceneUBO,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT
		);

		vkCmdUpdateBuffer(aCmdBuf, aSceneUBO, 0, sizeof(glsl::SceneUniform), &aSceneUniform);

		Utils::bufferBarrier(
			aCmdBuf,
			aSceneUBO,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		);

		// Clear attachments
		VkClearValue clearValues[2]{};
		clearValues[0].color.float32[0] = 0.1f;
		clearValues[0].color.float32[1] = 0.1f;
		clearValues[0].color.float32[2] = 0.1f;
		clearValues[0].color.float32[3] = 1.0f;

		clearValues[1].depthStencil.depth = 1.0f;

		// Initialise render pass
		VkRenderPassBeginInfo passInfo{};
		passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passInfo.renderPass = aRenderPass;
		passInfo.framebuffer = aFramebuffer;
		passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo.renderArea.extent = aExtent;
		passInfo.clearValueCount = 2;
		passInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(aCmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipeline);

		vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, 0, 1, &aSceneDescriptorSet, 0, nullptr);

		std::pair<void*, int> renderComponents = entityManager.GetComponents<RenderComponent>();
		int j;
		for (std::size_t i = 0; i < renderComponents.second; i++) {
			RenderComponent r = reinterpret_cast<RenderComponent*>(renderComponents.first)[i];
			int z = 0;
		}
		for (std::size_t i = 0; i < renderComponents.second; i++) {
			std::uint32_t offset = i * dynamicOffset;
			vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, 3, 1, &modelMatricesDescriptor, 1, &offset);
			j = reinterpret_cast<RenderComponent*>(renderComponents.first)[i].GetModelIndex();
			models[reinterpret_cast<RenderComponent*>(renderComponents.first)[i].GetModelIndex()].drawModel(aCmdBuf, aPipelineLayout);
		}

		vkCmdEndRenderPass(aCmdBuf);

		if (const auto res = vkEndCommandBuffer(aCmdBuf); VK_SUCCESS != res)
			throw Utils::Error("Unable to end command buffer\n vkEndCommandBuffer() returned %s", Utils::toString(res).c_str());
		
	}

	// For the sake of simplicity to get it up and running I will just use a separate method to render with deferred lighting
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
	) {
		// Begin recording
		beginCommandBuffer(aCmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		Utils::bufferBarrier(
			aCmdBuf,
			aSceneUBO,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT
		);

		vkCmdUpdateBuffer(aCmdBuf, aSceneUBO, 0, sizeof(glsl::SceneUniform), &aSceneUniform);

		Utils::bufferBarrier(
			aCmdBuf,
			aSceneUBO,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		);

		Utils::bufferBarrier(
			aCmdBuf,
			aLightsUBO,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT
		);

		vkCmdUpdateBuffer(aCmdBuf, aLightsUBO, 0, sizeof(glsl::LightsUniform), &aLightsUniform);

		Utils::bufferBarrier(
			aCmdBuf,
			aLightsUBO,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		);

		// Clear attachments
		VkClearValue clearValues[5]{};
		clearValues[0].color.float32[0] = 0.1f;
		clearValues[0].color.float32[1] = 0.1f;
		clearValues[0].color.float32[2] = 0.1f;
		clearValues[0].color.float32[3] = 1.0f;

		clearValues[1].color.float32[0] = 0.1f;
		clearValues[1].color.float32[1] = 0.1f;
		clearValues[1].color.float32[2] = 0.1f;
		clearValues[1].color.float32[3] = 1.0f;

		clearValues[2].color.float32[0] = 0.1f;
		clearValues[2].color.float32[1] = 0.1f;
		clearValues[2].color.float32[2] = 0.1f;
		clearValues[2].color.float32[3] = 1.0f;

		clearValues[3].color.float32[0] = 0.1f;
		clearValues[3].color.float32[1] = 0.1f;
		clearValues[3].color.float32[2] = 0.1f;
		clearValues[3].color.float32[3] = 1.0f;

		clearValues[4].depthStencil.depth = 1.0f;

		// Initialise render pass
		VkRenderPassBeginInfo passInfo{};
		passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passInfo.renderPass = aRenderPass;
		passInfo.framebuffer = aFramebuffer;
		passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo.renderArea.extent = aExtent;
		passInfo.clearValueCount = 5;
		passInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(aCmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aGBufPipeline);

		vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aGBufPipelineLayout, 0, 1, &aSceneDescriptorSet, 0, nullptr);

		std::pair<void*, int> renderComponents = entityManager.GetComponents<RenderComponent>();
		int j;
		for (std::size_t i = 0; i < renderComponents.second; i++) {
			RenderComponent r = reinterpret_cast<RenderComponent*>(renderComponents.first)[i];
			int z = 0;
		}
		for (std::size_t i = 0; i < renderComponents.second; i++) {
			std::uint32_t offset = i * dynamicOffset;
			vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aGBufPipelineLayout, 3, 1, &modelMatricesDescriptor, 1, &offset);
			j = reinterpret_cast<RenderComponent*>(renderComponents.first)[i].GetModelIndex();
			models[reinterpret_cast<RenderComponent*>(renderComponents.first)[i].GetModelIndex()].drawModel(aCmdBuf, aGBufPipelineLayout);
		}

		vkCmdNextSubpass(aCmdBuf, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aDeferredPipeline);
		vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aDeferredPipelineLayout, 0, 1, &aDeferredShadingDescriptor, 0, nullptr);
		vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aDeferredPipelineLayout, 1, 1, &aSceneDescriptorSet, 0, nullptr);
		vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aDeferredPipelineLayout, 2, 1, &aLightsDescriptor, 0, nullptr);

		vkCmdDraw(aCmdBuf, 3, 1, 0, 0);

		vkCmdEndRenderPass(aCmdBuf);

		if (const auto res = vkEndCommandBuffer(aCmdBuf); VK_SUCCESS != res)
			throw Utils::Error("Unable to end command buffer\n vkEndCommandBuffer() returned %s", Utils::toString(res).c_str());
	}

}
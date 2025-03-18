#include "Renderer.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "VulkanUtils.hpp"
#include "VulkanDevice.hpp"
#include "PipelineCreation.hpp"
#include "../ECS/RenderComponent.hpp"

#include "Utils.hpp"

namespace Engine {

	Renderer::Renderer(VulkanContext* aContext, EntityManager* entityManager) {
		this->context = aContext;
		this->entityManager = entityManager;
	}

	void Renderer::initialiseRenderer() {
		// Render passes
		this->renderPasses.emplace("default", createRenderPass(*context->window));
		this->renderPasses.emplace("deferred", createDeferredRenderPass(*context->window));
	
		// Descriptor Layouts
		std::vector<DescriptorSetting> sceneSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("sceneLayout", createDescriptorLayout(*context->window, sceneSetting));

		std::vector<DescriptorSetting> ssboSetting = { {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("SSBOLayout", createDescriptorLayout(*context->window, ssboSetting));

		std::vector<DescriptorSetting> fragUBOSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("fragUBOLayout", createDescriptorLayout(*context->window, fragUBOSetting));

		std::vector<DescriptorSetting> modelMatricesSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT} };
		this->descriptorLayouts.emplace("modelMatricesLayout", createDescriptorLayout(*context->window, modelMatricesSetting));

		std::vector<DescriptorSetting> deferredSetting = { {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT},
														{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT} ,
														{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT} ,
														{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("deferredLayout", createDescriptorLayout(*context->window, deferredSetting));

		std::vector<DescriptorSetting> materialSetting = { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT},
														{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} ,
														{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} ,
														{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} ,
														{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("materialLayout", createDescriptorLayout(*context->window, materialSetting));

		// Descriptor Layout groups (groups descriptor layouts for pipelines)
		std::vector<VkDescriptorSetLayout> defaultDescriptorLayouts;
		defaultDescriptorLayouts.emplace_back(this->descriptorLayouts["sceneLayout"].handle);
		defaultDescriptorLayouts.emplace_back(this->descriptorLayouts["materialLayout"].handle);
		defaultDescriptorLayouts.emplace_back(this->descriptorLayouts["SSBOLayout"].handle);
		defaultDescriptorLayouts.emplace_back(this->descriptorLayouts["modelMatricesLayout"].handle);

		std::vector<VkDescriptorSetLayout> deferredShadingLayouts;
		deferredShadingLayouts.emplace_back(this->descriptorLayouts["deferredLayout"].handle);
		deferredShadingLayouts.emplace_back(this->descriptorLayouts["sceneLayout"].handle);
		deferredShadingLayouts.emplace_back(this->descriptorLayouts["fragUBOLayout"].handle);

		// Pipeline layouts
		this->pipelineLayouts.emplace("default", createPipelineLayout(*context->window, defaultDescriptorLayouts, true));
		this->pipelineLayouts.emplace("deferred", createPipelineLayout(*context->window, deferredShadingLayouts, false));

		// Pipelines
		std::tuple<vk::Pipeline, vk::Pipeline> deferredPipelines = createDeferredPipelines(
			*context->window, 
			this->renderPasses["deferred"].handle, 
			this->pipelineLayouts["default"].handle, 
			this->pipelineLayouts["deferred"].handle);

		this->pipelines.emplace("default", createPipeline(*context->window, this->renderPasses["default"].handle, this->pipelineLayouts["default"].handle));
		this->pipelines.emplace("gBufWrite", std::move(std::get<0>(deferredPipelines)));
		this->pipelines.emplace("deferred", std::move(std::get<1>(deferredPipelines)));

		// Buffers
		TextureBufferSetting depthTexture = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT };
		this->buffers.emplace("depth", createTextureBuffer(*context, depthTexture));

		TextureBufferSetting colorTexture = {
			VK_FORMAT_R16G16B16A16_SFLOAT,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT };
		this->buffers.emplace("normals", createTextureBuffer(*context, colorTexture));
		this->buffers.emplace("albedo", createTextureBuffer(*context, colorTexture));
		this->buffers.emplace("emissive", createTextureBuffer(*context, colorTexture));

		// Framebuffers
		std::vector<VkImageView> defaultViews;
		defaultViews.emplace_back(this->buffers["depth"].second.handle);
		createFramebuffers(*context->window, this->defaultFramebuffers, this->renderPasses["default"].handle, defaultViews);
		
		std::vector<VkImageView> deferredViews;
		// Must be emplaced in the vector in the *EXACT* same order as defined in the render pass
		deferredViews.emplace_back(this->buffers["normals"].second.handle);
		deferredViews.emplace_back(this->buffers["albedo"].second.handle);
		deferredViews.emplace_back(this->buffers["emissive"].second.handle);
		deferredViews.emplace_back(this->buffers["depth"].second.handle);
		createFramebuffers(*context->window, this->deferredFramebuffers, this->renderPasses["deferred"].handle, deferredViews);

		// Setup synchronisation
		for (std::size_t i = 0; i < context->window->swapViews.size(); i++) {
			this->cmdBuffers.emplace_back(createCommandBuffer(*context->window));
			this->frameDone.emplace_back(createFence(*context->window, VK_FENCE_CREATE_SIGNALED_BIT));
			this->imageAvailable.emplace_back(createSemaphore(*context->window));
			this->renderFinished.emplace_back(createSemaphore(*context->window));
		}

		// Create uniform buffers
		this->uniformBuffers.emplace("scene", 
			vk::createBuffer(
				"sceneUBO", 
				*context->allocator, 
				sizeof(glsl::SceneUniform), 
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
				0, 
				VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE));
		this->uniformBuffers.emplace("lights",
			vk::createBuffer(
				"lightsUBO",
				*context->allocator,
				sizeof(glsl::LightsUniform),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				0,
				VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE));
		this->uniformBuffers.emplace("modelMatrices",
			this->createDynamicUniformBuffer());

		// Map memory for model matrices dynamic buffer
		vmaMapMemory(context->allocator->allocator, this->uniformBuffers["modelMatrices"].allocation, &this->uniformBuffers["modelMatrices"].mapped);

		// Descriptor sets
		this->descriptorSets.emplace("scene", 
			createUBODescriptor(
				*context->window, 
				this->descriptorLayouts["sceneLayout"].handle, 
				this->uniformBuffers["scene"].buffer));
		this->descriptorSets.emplace("lights", 
			createUBODescriptor(
				*context->window, 
				this->descriptorLayouts["fragUBOLayout"].handle, 
				this->uniformBuffers["lights"].buffer));
		this->descriptorSets.emplace("modelMatrices", 
			createModelMatricesDescriptor(
				*context->window, 
				this->descriptorLayouts["modelMatricesLayout"].handle, 
				this->uniformBuffers["modelMatrices"].buffer, 
				this->dynamicUBOAlignment));
		this->descriptorSets.emplace("deferredShading", 
			createDeferredShadingDescriptor(
				*context->window, 
				this->descriptorLayouts["deferredLayout"].handle, 
				this->buffers["depth"].second.handle, 
				this->buffers["normals"].second.handle, 
				this->buffers["albedo"].second.handle, 
				this->buffers["emissive"].second.handle));

		// Initialise light properties
		this->uniforms.lightsUniform.light[0].pos = glm::vec4(0.0f, 3.0f, 0.0f, 0.0f);
		this->uniforms.lightsUniform.light[1].pos = glm::vec4(-6.0f, 1.6f, -2.1f, 0.0f);
		this->uniforms.lightsUniform.light[2].pos = glm::vec4(-6.0f, 1.6f, 1.5f, 0.0f);
		this->uniforms.lightsUniform.light[3].pos = glm::vec4(4.8f, 1.6f, -2.1f, 0.0f);
		this->uniforms.lightsUniform.light[4].pos = glm::vec4(4.8f, 1.6f, 1.5f, 0.0f);
		this->uniforms.lightsUniform.light[0].color = glm::vec4(1.0f, 1.0f, 1.0f, 0.0f);
		this->uniforms.lightsUniform.light[1].color = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
		this->uniforms.lightsUniform.light[2].color = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		this->uniforms.lightsUniform.light[3].color = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
		this->uniforms.lightsUniform.light[4].color = glm::vec4(1.0f, 0.0f, 1.0f, 0.0f);
	}

	vk::Buffer Renderer::createDynamicUniformBuffer() {
		std::size_t uboAlignment = context->window->device->minUBOAlignment;
		this->dynamicUBOAlignment = (sizeof(glm::mat4) + uboAlignment - 1) & ~(uboAlignment - 1);

		VkDeviceSize bufferSize = this->dynamicUBOAlignment * entityManager->GetComponentTypeSize<RenderComponent>();

		this->uniforms.modelMatricesUniform.model = (glm::mat4*)Utils::allocAligned(bufferSize, this->dynamicUBOAlignment);

		return vk::createBuffer("dynamicUBO", *context->allocator, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	}

	void Renderer::attachCamera(Camera* camera) {
		this->camera = camera;
	}

	void Renderer::initialiseModelDescriptors(std::vector<vk::Model>& aModels) {
		for (vk::Model& model : aModels)
			model.createDescriptorSets(*context, this->descriptorLayouts["materialLayout"].handle, this->descriptorLayouts["SSBOLayout"].handle);
	}

	bool Renderer::checkSwapchain() {
		if (this->recreateSwapchain) {
			vkDeviceWaitIdle(context->window->device->device);

			// Recreate swapchain stuff

			const auto changes = Engine::recreateSwapchain(*context->window);

			if (changes.changedFormat)
				this->recreateFormatDependents();

			if (changes.changedSize)
				this->recreateSizeDependents();

			this->recreateOthers();

			this->recreateSwapchain = false;
			return true;
		}

		return false;
	}

	bool Renderer::acquireSwapchainImage() {
		this->frameIndex++;
		this->frameIndex %= this->cmdBuffers.size();

		waitForFences(*context->window, this->frameDone, this->frameIndex);

		if (acquireNextSwapchainImage(*context->window, this->imageAvailable, this->frameIndex, this->imageIndex)) {
			this->recreateSwapchain = true;

			--this->frameIndex;
			this->frameIndex %= this->cmdBuffers.size();

			return true;
		}

		resetFences(*context->window, this->frameDone, this->frameIndex);

		return false;
	}

	void Renderer::updateUniforms() {
		// Update scene uniforms
		float width = context->window->swapchainExtent.width;
		float height = context->window->swapchainExtent.height;
		const float aspectRatio = width / height;

		this->uniforms.sceneUniform.projection = glm::perspective(glm::radians(this->camera->fov), aspectRatio, this->camera->nearPlane, this->camera->farPlane);
		this->uniforms.sceneUniform.projection[1][1] *= -1.0f;
		this->uniforms.sceneUniform.view = glm::lookAt(this->camera->position, this->camera->position + this->camera->frontDirection, glm::vec3(0.0f, 1.0f, 0.0f));
		this->uniforms.sceneUniform.position = glm::vec4(this->camera->position, 1.0f);

		// Update model matrices
		std::vector<int> models = this->entityManager->GetEntitiesWithComponent<RenderComponent>();
		for (std::size_t i = 0; i < models.size(); i++) {
			glm::mat4* modelMatrix = (glm::mat4*)((std::uint64_t)this->uniforms.modelMatricesUniform.model + (i * this->dynamicUBOAlignment));

			// This will need to be changed to get a 'parent' model matrix, not
			// just the first node's model matrix.
			*modelMatrix = this->entityManager->GetEntity(models[i])->GetModelMatrix();
		}

		int size = this->entityManager->GetComponentTypeSize<RenderComponent>() * this->dynamicUBOAlignment;

		std::memcpy(this->uniformBuffers["modelMatrices"].mapped, this->uniforms.modelMatricesUniform.model, size);

		vmaFlushAllocation(context->allocator->allocator, this->uniformBuffers["modelMatrices"].allocation, 0, size);
	}

	void Renderer::render(RenderMode aRenderMode, std::vector<vk::Model>& models) {
		switch (aRenderMode) {
		case RenderMode::FORWARD:
			this->renderForward(models);
			break;
		case RenderMode::DEFERRED:
			this->renderDeferred(models);
			break;
		}
	}

	void Renderer::submitRender() {
		const VkResult result = submitAndPresent(*context->window, this->cmdBuffers, this->frameDone, this->imageAvailable, this->renderFinished, this->frameIndex, this->imageIndex);

		if (VK_SUBOPTIMAL_KHR == result || VK_ERROR_OUT_OF_DATE_KHR == result)
			this->recreateSwapchain = true;
		else if (VK_SUCCESS != result)
			throw Utils::Error("Unable to present swapchain image %u\n vkQueuePresentKHR() returned %s", this->imageIndex, Utils::toString(result).c_str());
	}

	void Renderer::finishRendering() {
		vkDeviceWaitIdle(context->window->device->device);

		vmaUnmapMemory(context->allocator->allocator, this->uniformBuffers["modelMatrices"].allocation);
	}

	void Renderer::renderForward(std::vector<vk::Model>& models) {
		VkCommandBuffer cmdBuf = this->cmdBuffers[this->frameIndex];

		// Begin recording
		beginCommandBuffer(cmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		Utils::bufferBarrier(
			cmdBuf,
			this->uniformBuffers["scene"].buffer,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT
		);

		vkCmdUpdateBuffer(cmdBuf, this->uniformBuffers["scene"].buffer, 0, sizeof(glsl::SceneUniform), &this->uniforms.sceneUniform);

		Utils::bufferBarrier(
			cmdBuf,
			this->uniformBuffers["scene"].buffer,
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
		passInfo.renderPass = this->renderPasses["default"].handle;
		passInfo.framebuffer = this->defaultFramebuffers[this->imageIndex].handle;
		passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo.renderArea.extent = context->window->swapchainExtent;
		passInfo.clearValueCount = 2;
		passInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["default"].handle);

		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["default"].handle, 0, 1, &this->descriptorSets["scene"], 0, nullptr);

		std::pair<void*, int> renderComponents = entityManager->GetComponents<RenderComponent>();
		for (std::size_t i = 0; i < renderComponents.second; i++) {
			RenderComponent r = reinterpret_cast<RenderComponent*>(renderComponents.first)[i];
		}
		for (std::size_t i = 0; i < renderComponents.second; i++) {
			std::uint32_t offset = i * this->dynamicUBOAlignment;
			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["default"].handle, 3, 1, &this->descriptorSets["modelMatrices"], 1, &offset);
			int j = reinterpret_cast<RenderComponent*>(renderComponents.first)[i].GetModelIndex();
			models[j].drawModel(cmdBuf, this->pipelineLayouts["default"].handle);
		}

		vkCmdEndRenderPass(cmdBuf);

		if (const auto res = vkEndCommandBuffer(cmdBuf); VK_SUCCESS != res)
			throw Utils::Error("Unable to end command buffer\n vkEndCommandBuffer() returned %s", Utils::toString(res).c_str());
	}

	void Renderer::renderDeferred(std::vector<vk::Model>& models) {
		VkCommandBuffer cmdBuf = this->cmdBuffers[this->frameIndex];

		// Begin recording
		beginCommandBuffer(cmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		Utils::bufferBarrier(
			cmdBuf,
			this->uniformBuffers["scene"].buffer,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT
		);

		vkCmdUpdateBuffer(cmdBuf, this->uniformBuffers["scene"].buffer, 0, sizeof(glsl::SceneUniform), &this->uniforms.sceneUniform);

		Utils::bufferBarrier(
			cmdBuf,
			this->uniformBuffers["scene"].buffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT
		);

		Utils::bufferBarrier(
			cmdBuf,
			this->uniformBuffers["lights"].buffer,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT
		);

		vkCmdUpdateBuffer(cmdBuf, this->uniformBuffers["lights"].buffer, 0, sizeof(glsl::LightsUniform), &this->uniforms.lightsUniform);

		Utils::bufferBarrier(
			cmdBuf,
			this->uniformBuffers["lights"].buffer,
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
		passInfo.renderPass = this->renderPasses["deferred"].handle;
		passInfo.framebuffer = this->deferredFramebuffers[this->imageIndex].handle;
		passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo.renderArea.extent = context->window->swapchainExtent;
		passInfo.clearValueCount = 5;
		passInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["gBufWrite"].handle);

		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["default"].handle, 0, 1, &this->descriptorSets["scene"], 0, nullptr);

		std::pair<void*, int> renderComponents = entityManager->GetComponents<RenderComponent>();
		for (std::size_t i = 0; i < renderComponents.second; i++) {
			RenderComponent r = reinterpret_cast<RenderComponent*>(renderComponents.first)[i];
		}
		for (std::size_t i = 0; i < renderComponents.second; i++) {
			std::uint32_t offset = i * this->dynamicUBOAlignment;
			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["default"].handle, 3, 1, &this->descriptorSets["modelMatrices"], 1, &offset);
			int j = reinterpret_cast<RenderComponent*>(renderComponents.first)[i].GetModelIndex();
			models[j].drawModel(cmdBuf, this->pipelineLayouts["default"].handle);
		}

		vkCmdNextSubpass(cmdBuf, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["deferred"].handle);
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["deferred"].handle, 0, 1, &this->descriptorSets["deferredShading"], 0, nullptr);
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["deferred"].handle, 1, 1, &this->descriptorSets["scene"], 0, nullptr);
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["deferred"].handle, 2, 1, &this->descriptorSets["lights"], 0, nullptr);

		vkCmdDraw(cmdBuf, 3, 1, 0, 0);

		vkCmdEndRenderPass(cmdBuf);

		if (const auto res = vkEndCommandBuffer(cmdBuf); VK_SUCCESS != res)
			throw Utils::Error("Unable to end command buffer\n vkEndCommandBuffer() returned %s", Utils::toString(res).c_str());
	}

	void Renderer::recreateFormatDependents() {
		this->renderPasses["default"] = createRenderPass(*context->window);
		this->renderPasses["deferred"] = createDeferredRenderPass(*context->window);
	}

	void Renderer::recreateSizeDependents() {
		TextureBufferSetting depthTexture = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT };
		this->buffers["depth"] = createTextureBuffer(*context, depthTexture);

		TextureBufferSetting colorTexture = {
			VK_FORMAT_R16G16B16A16_SFLOAT,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT };
		this->buffers["normals"] = createTextureBuffer(*context, colorTexture);
		this->buffers["albedo"] = createTextureBuffer(*context, colorTexture);
		this->buffers["emissive"] = createTextureBuffer(*context, colorTexture);
	
		std::tuple<vk::Pipeline, vk::Pipeline> deferredPipelines = createDeferredPipelines(
			*context->window,
			this->renderPasses["deferred"].handle,
			this->pipelineLayouts["default"].handle,
			this->pipelineLayouts["deferred"].handle);

		this->pipelines["default"] = createPipeline(*context->window, this->renderPasses["default"].handle, this->pipelineLayouts["default"].handle);
		this->pipelines["gBufWrite"] = std::move(std::get<0>(deferredPipelines));
		this->pipelines["deferred"] = std::move(std::get<1>(deferredPipelines));
	}

	void Renderer::recreateOthers() {
		this->defaultFramebuffers.clear();
		std::vector<VkImageView> defaultViews;
		defaultViews.emplace_back(this->buffers["depth"].second.handle);
		createFramebuffers(*context->window, this->defaultFramebuffers, this->renderPasses["default"].handle, defaultViews);

		this->deferredFramebuffers.clear();
		std::vector<VkImageView> deferredViews;
		deferredViews.emplace_back(this->buffers["normals"].second.handle);
		deferredViews.emplace_back(this->buffers["albedo"].second.handle);
		deferredViews.emplace_back(this->buffers["emissive"].second.handle);
		deferredViews.emplace_back(this->buffers["depth"].second.handle);
		createFramebuffers(*context->window, this->deferredFramebuffers, this->renderPasses["deferred"].handle, deferredViews);
	
		this->descriptorSets["deferredShading"] = createDeferredShadingDescriptor(
			*context->window,
			this->descriptorLayouts["deferredLayout"].handle,
			this->buffers["depth"].second.handle,
			this->buffers["normals"].second.handle,
			this->buffers["albedo"].second.handle,
			this->buffers["emissive"].second.handle);
	}
}
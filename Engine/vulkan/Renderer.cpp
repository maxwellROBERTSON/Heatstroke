#include "Renderer.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "VulkanUtils.hpp"
#include "VulkanDevice.hpp"
#include "PipelineCreation.hpp"
#include "../ECS/RenderComponent.hpp"

#include "Utils.hpp"

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>

namespace Engine {

	Renderer::Renderer(VulkanContext* aContext, EntityManager* entityManager, Engine::Game* game) {
		this->context = aContext;
		this->entityManager = entityManager;
		this->game = game;
	}

	VkRenderPass& Renderer::GetRenderPass(std::string s)
	{
		return this->renderPasses[s].handle;
	}

	void Renderer::initialiseRenderer() {
		// Render passes
		this->renderPasses.emplace("forward", createRenderPass(*this->context->window));
		this->renderPasses.emplace("deferred", createDeferredRenderPass(*this->context->window));
		this->renderPasses.emplace("shadow", createShadowRenderPass(*this->context->window));

		// Descriptor Layouts
		std::vector<DescriptorSetting> sceneSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("sceneLayout", createDescriptorLayout(*this->context->window, sceneSetting));

		std::vector<DescriptorSetting> ssboSetting = { {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("SSBOLayout", createDescriptorLayout(*this->context->window, ssboSetting));

		std::vector<DescriptorSetting> vertUBOSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT} };
		this->descriptorLayouts.emplace("vertUBOLayout", createDescriptorLayout(*this->context->window, vertUBOSetting));

		std::vector<DescriptorSetting> fragUBOSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("fragUBOLayout", createDescriptorLayout(*this->context->window, fragUBOSetting));

		std::vector<DescriptorSetting> modelMatricesSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC, VK_SHADER_STAGE_VERTEX_BIT} };
		this->descriptorLayouts.emplace("modelMatricesLayout", createDescriptorLayout(*this->context->window, modelMatricesSetting));

		std::vector<DescriptorSetting> deferredSetting = { {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT},
														{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT} ,
														{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT} ,
														{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("deferredLayout", createDescriptorLayout(*this->context->window, deferredSetting));

		std::vector<DescriptorSetting> materialSetting = { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}, // Base colour texture
														{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} ,   // Metalness/Roughness texture
														{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} ,   // Emissive texure
														{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} ,   // Occlusion texture
														{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} };  // Normal texture
		this->descriptorLayouts.emplace("materialLayout", createDescriptorLayout(*this->context->window, materialSetting));

		std::vector<DescriptorSetting> shadowMapSetting = { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} };  // Shadow map
		this->descriptorLayouts.emplace("shadowMapLayout", createDescriptorLayout(*this->context->window, shadowMapSetting));

		// Descriptor Layout groups (groups descriptor layouts for pipelines)

		std::vector<VkDescriptorSetLayout> forwardLayouts;
		forwardLayouts.emplace_back(this->descriptorLayouts["sceneLayout"].handle);
		forwardLayouts.emplace_back(this->descriptorLayouts["materialLayout"].handle);
		forwardLayouts.emplace_back(this->descriptorLayouts["SSBOLayout"].handle);
		forwardLayouts.emplace_back(this->descriptorLayouts["modelMatricesLayout"].handle);
		forwardLayouts.emplace_back(this->descriptorLayouts["vertUBOLayout"].handle);
		forwardLayouts.emplace_back(this->descriptorLayouts["shadowMapLayout"].handle);

		std::vector<VkDescriptorSetLayout> forwardshadowLayouts;
		forwardshadowLayouts.emplace_back(this->descriptorLayouts["sceneLayout"].handle);
		forwardshadowLayouts.emplace_back(this->descriptorLayouts["materialLayout"].handle);
		forwardshadowLayouts.emplace_back(this->descriptorLayouts["SSBOLayout"].handle);
		forwardshadowLayouts.emplace_back(this->descriptorLayouts["modelMatricesLayout"].handle);
		forwardshadowLayouts.emplace_back(this->descriptorLayouts["vertUBOLayout"].handle);

		std::vector<VkDescriptorSetLayout> deferredShadingLayouts;
		deferredShadingLayouts.emplace_back(this->descriptorLayouts["deferredLayout"].handle);
		deferredShadingLayouts.emplace_back(this->descriptorLayouts["sceneLayout"].handle);
		deferredShadingLayouts.emplace_back(this->descriptorLayouts["fragUBOLayout"].handle);

		std::vector<VkDescriptorSetLayout> shadowLayout;
		shadowLayout.emplace_back(this->descriptorLayouts["vertUBOLayout"].handle);
		shadowLayout.emplace_back(this->descriptorLayouts["modelMatricesLayout"].handle);

		// Pipeline layouts
		this->pipelineLayouts.emplace("forward", createPipelineLayout(*this->context->window, forwardLayouts, true));
		this->pipelineLayouts.emplace("forwardshadow", createPipelineLayout(*this->context->window, forwardshadowLayouts, true));
		this->pipelineLayouts.emplace("deferred", createPipelineLayout(*this->context->window, deferredShadingLayouts, false));
		this->pipelineLayouts.emplace("shadow", createPipelineLayout(*this->context->window, shadowLayout, false));

		// Pipelines
		std::tuple<vk::Pipeline, vk::Pipeline> deferredPipelines = createDeferredPipelines(
			*this->context->window,
			this->renderPasses["deferred"].handle,
			this->pipelineLayouts["forward"].handle,
			this->pipelineLayouts["deferred"].handle);

		this->pipelines.emplace("forward", createPipeline(*this->context->window, this->renderPasses["forward"].handle, this->pipelineLayouts["forward"].handle));
		this->pipelines.emplace("forwardshadow", createPipeline(*this->context->window, this->renderPasses["forward"].handle, this->pipelineLayouts["forwardshadow"].handle));
		this->pipelines.emplace("gBufWrite", std::move(std::get<0>(deferredPipelines)));
		this->pipelines.emplace("deferred", std::move(std::get<1>(deferredPipelines)));
		this->pipelines.emplace("shadow", createShadowOffscreenPipeline(*this->context->window, this->renderPasses["shadow"].handle, this->pipelineLayouts["shadow"].handle));

		// Buffers
		TextureBufferSetting depthTexture = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT,
			this->context->window->swapchainExtent };
		this->buffers.emplace("depth", createTextureBuffer(*this->context, depthTexture));

		TextureBufferSetting colorTexture = {
			VK_FORMAT_R16G16B16A16_SFLOAT,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			this->context->window->swapchainExtent };
		this->buffers.emplace("normals", createTextureBuffer(*this->context, colorTexture));
		this->buffers.emplace("albedo", createTextureBuffer(*this->context, colorTexture));
		this->buffers.emplace("emissive", createTextureBuffer(*this->context, colorTexture));

		TextureBufferSetting shadowDepthTexture = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT,
			VkExtent2D {2048, 2048} }; // Probably will want to make the shadow map resolution more easily editable, rather than hardcoding it everywhere
		this->buffers.emplace("shadowDepth", createTextureBuffer(*this->context, shadowDepthTexture));

		// Framebuffers
		VkExtent2D swapchainExtent = this->context->window->swapchainExtent;
		VkExtent2D shadowExtent = VkExtent2D{ 2048, 2048 };

		std::vector<VkImageView> forwardViews;
		forwardViews.emplace_back(this->buffers["depth"].second.handle);
		createFramebuffers(*this->context->window, *this->framebuffersMap[FORWARD], this->renderPasses["forward"].handle, forwardViews, swapchainExtent);

		std::vector<VkImageView> deferredViews;
		// Must be emplaced in the vector in the *EXACT* same order as defined in the render pass
		deferredViews.emplace_back(this->buffers["normals"].second.handle);
		deferredViews.emplace_back(this->buffers["albedo"].second.handle);
		deferredViews.emplace_back(this->buffers["emissive"].second.handle);
		deferredViews.emplace_back(this->buffers["depth"].second.handle);
		createFramebuffers(*this->context->window, *this->framebuffersMap[DEFERRED], this->renderPasses["deferred"].handle, deferredViews, swapchainExtent);

		std::vector<VkImageView> shadowViews;
		shadowViews.emplace_back(this->buffers["shadowDepth"].second.handle);
		createFramebuffers(*this->context->window, *this->framebuffersMap[SHADOWS], this->renderPasses["shadow"].handle, shadowViews, shadowExtent, true);

		// Setup synchronisation
		for (std::size_t i = 0; i < context->window->swapViews.size(); i++) {
			this->cmdBuffers.emplace_back(createCommandBuffer(*this->context->window));
			this->frameDone.emplace_back(createFence(*this->context->window, VK_FENCE_CREATE_SIGNALED_BIT));
			this->imageAvailable.emplace_back(createSemaphore(*this->context->window));
			this->renderFinished.emplace_back(createSemaphore(*this->context->window));
		}

		// Create uniform buffers
		this->uniformBuffers.emplace("scene", 
			vk::createBuffer(
				"sceneUBO", 
				*this->context->allocator,
				sizeof(glsl::SceneUniform), 
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 
				0, 
				VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE));
		this->uniformBuffers.emplace("lights",
			vk::createBuffer(
				"lightsUBO",
				*this->context->allocator,
				sizeof(glsl::LightsUniform),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				0,
				VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE));
		this->uniformBuffers.emplace("depthMVP",
			vk::createBuffer(
				"depthMVP",
				*this->context->allocator,
				sizeof(glsl::DepthMVP),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				0,
				VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE));

		// Samplers
		vk::SamplerInfo samplerInfo = {
			VK_FILTER_LINEAR,
			VK_FILTER_LINEAR,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE };
		this->depthSampler = createTextureSampler(*this->context->window, samplerInfo);

		// Descriptor sets
		this->descriptorSets.emplace("scene", 
			createUBODescriptor(
				*this->context->window,
				this->descriptorLayouts["sceneLayout"].handle,
				this->uniformBuffers["scene"].buffer));
		this->descriptorSets.emplace("lights",
			createUBODescriptor(
				*this->context->window,
				this->descriptorLayouts["fragUBOLayout"].handle,
				this->uniformBuffers["lights"].buffer));
		this->descriptorSets.emplace("deferredShading", 
			createDeferredShadingDescriptor(
				*this->context->window,
				this->descriptorLayouts["deferredLayout"].handle,
				this->buffers["depth"].second.handle,
				this->buffers["normals"].second.handle,
				this->buffers["albedo"].second.handle,
				this->buffers["emissive"].second.handle));
		this->descriptorSets.emplace("shadow",
			createUBODescriptor(
				*this->context->window,
				this->descriptorLayouts["vertUBOLayout"].handle,
				this->uniformBuffers["depthMVP"].buffer));
		this->descriptorSets.emplace("shadowMap",
			createImageDescriptor(
				*this->context->window,
				this->descriptorLayouts["shadowMapLayout"].handle,
				this->buffers["shadowDepth"].second.handle,
				this->depthSampler.handle));

		// Initialise light properties
		this->uniforms.lightsUniform.light[0].pos = glm::vec4(0.0f, 20.0f, 0.0f, 0.0f);
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

	void Renderer::initialiseModelMatrices()
	{
		this->uniformBuffers.emplace("modelMatrices",
			this->createDynamicUniformBuffer());

		// Map memory for model matrices dynamic buffer
		vmaMapMemory(context->allocator->allocator, this->uniformBuffers["modelMatrices"].allocation, &this->uniformBuffers["modelMatrices"].mapped);

		this->descriptorSets.emplace("modelMatrices",
			createModelMatricesDescriptor(
				*context->window,
				this->descriptorLayouts["modelMatricesLayout"].handle,
				this->uniformBuffers["modelMatrices"].buffer,
				this->dynamicUBOAlignment));

		isSceneLoaded = true;
	}

	void Renderer::cleanModelMatrices()
	{
		vmaUnmapMemory(context->allocator->allocator, this->uniformBuffers["modelMatrices"].allocation);

		//this->descriptorSets.erase("sceneDescriptors");
		this->descriptorSets.erase("modelMatricesDescriptor");

		isSceneLoaded = false;
	}

	vk::Buffer Renderer::createDynamicUniformBuffer() {
		std::size_t uboAlignment = this->context->window->device->minUBOAlignment;
		this->dynamicUBOAlignment = (sizeof(glm::mat4) + uboAlignment - 1) & ~(uboAlignment - 1);

		VkDeviceSize bufferSize = this->dynamicUBOAlignment * entityManager->GetComponentTypeSize<RenderComponent>();

		this->uniforms.modelMatricesUniform.model = (glm::mat4*)Utils::allocAligned(bufferSize, this->dynamicUBOAlignment);

		return vk::createBuffer("dynamicUBO", *this->context->allocator, bufferSize, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);
	}

	void Renderer::attachCamera(Camera* camera) {
		this->camera = camera;
	}

	void Renderer::initialiseModelDescriptors(std::vector<vk::Model>& aModels) {
		for (vk::Model& model : aModels)
			model.createDescriptorSets(*this->context, this->descriptorLayouts["materialLayout"].handle, this->descriptorLayouts["SSBOLayout"].handle);
	}

	bool Renderer::checkSwapchain() {
		if (this->recreateSwapchain) {
			vkDeviceWaitIdle(this->context->window->device->device);

			// Recreate swapchain stuff

			const auto changes = Engine::recreateSwapchain(*this->context->window);

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

		waitForFences(*this->context->window, this->frameDone, this->frameIndex);

		if (acquireNextSwapchainImage(*this->context->window, this->imageAvailable, this->frameIndex, this->imageIndex)) {
			this->recreateSwapchain = true;

			--this->frameIndex;
			this->frameIndex %= this->cmdBuffers.size();

			return true;
		}

		resetFences(*this->context->window, this->frameDone, this->frameIndex);

		return false;
	}

	void Renderer::updateUniforms() {
		// Update scene uniforms
		float width = this->context->window->swapchainExtent.width;
		float height = this->context->window->swapchainExtent.height;
		const float aspectRatio = width / height;

		this->uniforms.sceneUniform.projection = glm::perspective(glm::radians(this->camera->fov), aspectRatio, this->camera->nearPlane, this->camera->farPlane);
		this->uniforms.sceneUniform.projection[1][1] *= -1.0f;
		this->uniforms.sceneUniform.view = glm::lookAt(this->camera->position, this->camera->position + this->camera->frontDirection, glm::vec3(0.0f, 1.0f, 0.0f));
		this->uniforms.sceneUniform.position = glm::vec4(this->camera->position, 1.0f);

		// Update shadow depth MVP
		glm::mat4 depthProjection = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 96.0f);
		glm::mat4 depthView = glm::lookAt(glm::vec3(0.75f, 20.0f, -0.4f), glm::vec3(0.75f, 0.0f, -0.4f), glm::vec3(1.0f, 0.0f, 0.0f));
		//glm::mat4 depthModel = glm::mat4(1.0f); // Model matrix is just identity

		this->uniforms.depthMVP.depthMVP = depthProjection * depthView;

		if (isSceneLoaded)
			updateModelMatrices();
	}

	void Renderer::updateModelMatrices()
	{
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

		vmaFlushAllocation(this->context->allocator->allocator, this->uniformBuffers["modelMatrices"].allocation, 0, size);
	}

	void Renderer::render(std::vector<vk::Model>& models) {
		unsigned int modes = (*game->GetRenderModes());
		if ((modes & (1 << GUIHOME)))
		{
			this->renderGUI();
		}
		else if ((modes & (1 << GUISETTINGS)))
		{
			this->renderGUI();
		}
		else if (((modes & (1 << FORWARD))))
		{
			this->renderForwardShadows(models, (modes & (1 << GUIDEBUG)));
			/*if (((modes & (1 << SHADOWS))))
				this->renderForwardShadows(models, (modes & (1 << GUIDEBUG)));
			else
				this->renderForward(models, (modes & (1 << GUIDEBUG)));*/
		}
		else if ((modes & (1 << DEFERRED)))
		{
			this->renderDeferred(models, ((*game->GetRenderModes()) & (1 << GUIDEBUG)));
		}
		else
		{
			std::cerr << "Unknown RenderMode Config" << std::endl;
		}
	}

	void Renderer::submitRender() {
		const VkResult result = submitAndPresent(*this->context->window, this->cmdBuffers, this->frameDone, this->imageAvailable, this->renderFinished, this->frameIndex, this->imageIndex);

		if (VK_SUBOPTIMAL_KHR == result || VK_ERROR_OUT_OF_DATE_KHR == result)
			this->recreateSwapchain = true;
		else if (VK_SUCCESS != result)
			throw Utils::Error("Unable to present swapchain image %u\n vkQueuePresentKHR() returned %s", this->imageIndex, Utils::toString(result).c_str());
	}

	void Renderer::finishRendering() {
		vkDeviceWaitIdle(this->context->window->device->device);

		vmaUnmapMemory(this->context->allocator->allocator, this->uniformBuffers["modelMatrices"].allocation);
	}

	void Renderer::modeOn(Engine::RenderMode r)
	{
		VkExtent2D swapchainExtent = this->context->window->swapchainExtent;

		this->framebuffersMap[r]->clear();
		std::vector<VkImageView> views;

		if (r == FORWARD)
		{
			views.emplace_back(this->buffers["depth"].second.handle);
			createFramebuffers(*this->context->window, *this->framebuffersMap[FORWARD], this->renderPasses["default"].handle, views, swapchainExtent);
		}
		else if (r == DEFERRED)
		{
			views.emplace_back(this->buffers["normals"].second.handle);
			views.emplace_back(this->buffers["albedo"].second.handle);
			views.emplace_back(this->buffers["emissive"].second.handle);
			views.emplace_back(this->buffers["depth"].second.handle);
			createFramebuffers(*this->context->window, *this->framebuffersMap[DEFERRED], this->renderPasses["deferred"].handle, views, swapchainExtent);

			this->descriptorSets["deferredShading"] = createDeferredShadingDescriptor(
				*this->context->window,
				this->descriptorLayouts["deferredLayout"].handle,
				this->buffers["depth"].second.handle,
				this->buffers["normals"].second.handle,
				this->buffers["albedo"].second.handle,
				this->buffers["emissive"].second.handle);
		}
		
		if (r == SHADOWS)
		{
			views.clear();
			VkExtent2D shadowExtent = VkExtent2D{ 2048, 2048 };
			views.emplace_back(this->buffers["shadowDepth"].second.handle);
			createFramebuffers(*this->context->window, *this->framebuffersMap[SHADOWS], this->renderPasses["shadow"].handle, views, shadowExtent, true);

			this->descriptorSets["shadowMap"] = createImageDescriptor(
				*this->context->window,
				this->descriptorLayouts["shadowMapLayout"].handle,
				this->buffers["shadowDepth"].second.handle,
				this->depthSampler.handle);
		}
	}

	void Renderer::modeOff(Engine::RenderMode r)
	{
		this->framebuffersMap[r]->clear();
	}

	void Renderer::renderGUI()
	{
		VkCommandBuffer cmdBuf = this->cmdBuffers[this->frameIndex];

		// Begin recording
		beginCommandBuffer(cmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

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
		passInfo.renderPass = this->renderPasses["forward"].handle;
		passInfo.framebuffer = (*this->framebuffersMap[FORWARD])[this->imageIndex].handle;
		passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo.renderArea.extent = context->window->swapchainExtent;
		passInfo.clearValueCount = 2;
		passInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);

		vkCmdEndRenderPass(cmdBuf);

		if (const auto res = vkEndCommandBuffer(cmdBuf); VK_SUCCESS != res)
			throw Utils::Error("Unable to end command buffer\n vkEndCommandBuffer() returned %s", Utils::toString(res).c_str());
	}

	void Renderer::renderForward(std::vector<vk::Model>& models, bool debug) {
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
		passInfo.renderPass = this->renderPasses["forward"].handle;
		passInfo.framebuffer = (*this->framebuffersMap[FORWARD])[this->imageIndex].handle;
		passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo.renderArea.extent = this->context->window->swapchainExtent;
		passInfo.clearValueCount = 2;
		passInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["forward"].handle);

		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"].handle, 0, 1, &this->descriptorSets["scene"], 0, nullptr);

		std::pair<void*, int> renderComponents = entityManager->GetComponents<RenderComponent>();
		for (std::size_t i = 0; i < renderComponents.second; i++) {
			RenderComponent r = reinterpret_cast<RenderComponent*>(renderComponents.first)[i];
		}
		for (std::size_t i = 0; i < renderComponents.second; i++) {
			std::uint32_t offset = i * this->dynamicUBOAlignment;
			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"].handle, 3, 1, &this->descriptorSets["modelMatrices"], 1, &offset);
			int j = reinterpret_cast<RenderComponent*>(renderComponents.first)[i].GetModelIndex();
			models[j].drawModel(cmdBuf, this->pipelineLayouts["forward"].handle);
		}

		if (debug)
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);

		vkCmdEndRenderPass(cmdBuf);

		if (const auto res = vkEndCommandBuffer(cmdBuf); VK_SUCCESS != res)
			throw Utils::Error("Unable to end command buffer\n vkEndCommandBuffer() returned %s", Utils::toString(res).c_str());
	}

	void Renderer::renderForwardShadows(std::vector<vk::Model>& models, bool debug) {
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
			this->uniformBuffers["depthMVP"].buffer,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT
		);

		vkCmdUpdateBuffer(cmdBuf, this->uniformBuffers["depthMVP"].buffer, 0, sizeof(glsl::DepthMVP), &this->uniforms.depthMVP);

		Utils::bufferBarrier(
			cmdBuf,
			this->uniformBuffers["depthMVP"].buffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
		);

		VkClearValue clearValueS[1]{};
		clearValueS[0].depthStencil = { 1.0f, 0 };

		VkRenderPassBeginInfo passInfoS{};
		passInfoS.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passInfoS.renderPass = this->renderPasses["shadow"].handle;
		passInfoS.framebuffer = (*this->framebuffersMap[SHADOWS])[0].handle;
		passInfoS.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfoS.renderArea.extent = VkExtent2D{ 2048, 2048 };
		passInfoS.clearValueCount = 1;
		passInfoS.pClearValues = clearValueS;

		vkCmdBeginRenderPass(cmdBuf, &passInfoS, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["shadow"].handle);

		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["shadow"].handle, 0, 1, &this->descriptorSets["shadow"], 0, nullptr);

		std::pair<void*, int> renderComponentsS = entityManager->GetComponents<RenderComponent>();
		for (std::size_t i = 0; i < renderComponentsS.second; i++) {
			RenderComponent r = reinterpret_cast<RenderComponent*>(renderComponentsS.first)[i];
		}
		for (std::size_t i = 0; i < renderComponentsS.second; i++) {
			std::uint32_t offset = i * this->dynamicUBOAlignment;
			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["shadow"].handle, 1, 1, &this->descriptorSets["modelMatrices"], 1, &offset);
			int j = reinterpret_cast<RenderComponent*>(renderComponentsS.first)[i].GetModelIndex();
			models[j].drawModel(cmdBuf, this->pipelineLayouts["shadow"].handle, true);
		}
		//drawModels(cmdBuf, models, "shadow");

		vkCmdEndRenderPass(cmdBuf);

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
		passInfo.renderPass = this->renderPasses["forward"].handle;
		passInfo.framebuffer = (*this->framebuffersMap[FORWARD])[this->imageIndex].handle;
		passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo.renderArea.extent = this->context->window->swapchainExtent;
		passInfo.clearValueCount = 2;
		passInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["forwardshadow"].handle);

		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forwardshadow"].handle, 0, 1, &this->descriptorSets["scene"], 0, nullptr);
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forwardshadow"].handle, 4, 1, &this->descriptorSets["shadow"], 0, nullptr);
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forwardshadow"].handle, 5, 1, &this->descriptorSets["shadowMap"], 0, nullptr);

		drawModels(cmdBuf, models, "forward");

		if (debug)
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);

		vkCmdEndRenderPass(cmdBuf);

		if (const auto res = vkEndCommandBuffer(cmdBuf); VK_SUCCESS != res)
			throw Utils::Error("Unable to end command buffer\n vkEndCommandBuffer() returned %s", Utils::toString(res).c_str());
	}

	void Renderer::renderDeferred(std::vector<vk::Model>& models, bool debug) {
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
		passInfo.framebuffer = (*this->framebuffersMap[DEFERRED])[this->imageIndex].handle;
		passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo.renderArea.extent = this->context->window->swapchainExtent;
		passInfo.clearValueCount = 5;
		passInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["gBufWrite"].handle);

		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["default"].handle, 0, 1, &this->descriptorSets["scene"], 0, nullptr);

		drawModels(cmdBuf, models, "default");

		vkCmdNextSubpass(cmdBuf, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["deferred"].handle);
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["deferred"].handle, 0, 1, &this->descriptorSets["deferredShading"], 0, nullptr);
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["deferred"].handle, 1, 1, &this->descriptorSets["scene"], 0, nullptr);
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["deferred"].handle, 2, 1, &this->descriptorSets["lights"], 0, nullptr);

		vkCmdDraw(cmdBuf, 3, 1, 0, 0);

		if (debug)
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);

		vkCmdEndRenderPass(cmdBuf);

		if (const auto res = vkEndCommandBuffer(cmdBuf); VK_SUCCESS != res)
			throw Utils::Error("Unable to end command buffer\n vkEndCommandBuffer() returned %s", Utils::toString(res).c_str());
	}

	void Renderer::recreateFormatDependents() {
		this->renderPasses["forward"] = createRenderPass(*this->context->window);
		this->renderPasses["deferred"] = createDeferredRenderPass(*this->context->window);
		this->renderPasses["shadow"] = createShadowRenderPass(*this->context->window);
	}

	void Renderer::recreateSizeDependents() {
		TextureBufferSetting depthTexture = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT,
			this->context->window->swapchainExtent };
		this->buffers["depth"] = createTextureBuffer(*this->context, depthTexture);

		TextureBufferSetting colorTexture = {
			VK_FORMAT_R16G16B16A16_SFLOAT,
			VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			VK_IMAGE_ASPECT_COLOR_BIT,
			this->context->window->swapchainExtent };
		this->buffers["normals"] = createTextureBuffer(*this->context, colorTexture);
		this->buffers["albedo"] = createTextureBuffer(*this->context, colorTexture);
		this->buffers["emissive"] = createTextureBuffer(*this->context, colorTexture);

		TextureBufferSetting shadowDepthTexture = {
			VK_FORMAT_D32_SFLOAT_S8_UINT,
			VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			VK_IMAGE_ASPECT_DEPTH_BIT,
			VkExtent2D {2048, 2048} };
		this->buffers["shadowDepth"] = createTextureBuffer(*this->context, shadowDepthTexture);
	
		std::tuple<vk::Pipeline, vk::Pipeline> deferredPipelines = createDeferredPipelines(
			*this->context->window,
			this->renderPasses["deferred"].handle,
			this->pipelineLayouts["foward"].handle,
			this->pipelineLayouts["deferred"].handle);

		this->pipelines.emplace("forward", createPipeline(*this->context->window, this->renderPasses["forward"].handle, this->pipelineLayouts["forward"].handle));
		this->pipelines.emplace("forwardshadow", createPipeline(*this->context->window, this->renderPasses["forward"].handle, this->pipelineLayouts["forwardshadow"].handle));
		this->pipelines.emplace("gBufWrite", std::move(std::get<0>(deferredPipelines)));
		this->pipelines.emplace("deferred", std::move(std::get<1>(deferredPipelines)));
		this->pipelines.emplace("shadow", createShadowOffscreenPipeline(*this->context->window, this->renderPasses["shadow"].handle, this->pipelineLayouts["shadow"].handle));

	}

	void Renderer::recreateOthers() {
		VkExtent2D swapchainExtent = this->context->window->swapchainExtent;
		VkExtent2D shadowExtent = VkExtent2D{ 2048, 2048 };

		this->framebuffersMap[FORWARD]->clear();
		this->framebuffersMap[DEFERRED]->clear();
		this->framebuffersMap[SHADOWS]->clear();

		std::vector<VkImageView> forwardViews;
		forwardViews.emplace_back(this->buffers["depth"].second.handle);
		createFramebuffers(*this->context->window, *this->framebuffersMap[FORWARD], this->renderPasses["forward"].handle, forwardViews, swapchainExtent);

		std::vector<VkImageView> deferredViews;
		// Must be emplaced in the vector in the *EXACT* same order as defined in the render pass
		deferredViews.emplace_back(this->buffers["normals"].second.handle);
		deferredViews.emplace_back(this->buffers["albedo"].second.handle);
		deferredViews.emplace_back(this->buffers["emissive"].second.handle);
		deferredViews.emplace_back(this->buffers["depth"].second.handle);
		createFramebuffers(*this->context->window, *this->framebuffersMap[DEFERRED], this->renderPasses["deferred"].handle, deferredViews, swapchainExtent);

		std::vector<VkImageView> shadowViews;
		shadowViews.emplace_back(this->buffers["shadowDepth"].second.handle);
		createFramebuffers(*this->context->window, *this->framebuffersMap[SHADOWS], this->renderPasses["shadow"].handle, shadowViews, shadowExtent, true);

		this->descriptorSets["deferredShading"] = createDeferredShadingDescriptor(
			*this->context->window,
			this->descriptorLayouts["deferredLayout"].handle,
			this->buffers["depth"].second.handle,
			this->buffers["normals"].second.handle,
			this->buffers["albedo"].second.handle,
			this->buffers["emissive"].second.handle);
		this->descriptorSets["shadowMap"] = createImageDescriptor(
			*this->context->window,
			this->descriptorLayouts["shadowMapLayout"].handle,
			this->buffers["shadowDepth"].second.handle,
			this->depthSampler.handle);
	}

	void Renderer::drawModels(VkCommandBuffer& cmdBuf, std::vector<vk::Model>& models, std::string handle)
	{
		std::pair<void*, int> renderComponents = entityManager->GetComponents<RenderComponent>();
		for (std::size_t i = 0; i < renderComponents.second; i++) {
			RenderComponent r = reinterpret_cast<RenderComponent*>(renderComponents.first)[i];
		}
		for (std::size_t i = 0; i < renderComponents.second; i++) {
			std::uint32_t offset = i * this->dynamicUBOAlignment;
			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts[handle].handle, 3, 1, &this->descriptorSets["modelMatrices"], 1, &offset);
			int j = reinterpret_cast<RenderComponent*>(renderComponents.first)[i].GetModelIndex();
			models[j].drawModel(cmdBuf, this->pipelineLayouts[handle].handle);
		}
	}
}
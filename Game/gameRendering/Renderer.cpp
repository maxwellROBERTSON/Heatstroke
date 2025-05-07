#include "Renderer.hpp"

#include <numeric>
#include <iomanip>

#include "Engine/vulkan/VulkanContext.hpp"
#include "Engine/vulkan/VulkanDevice.hpp"
#include "Engine/vulkan/PipelineCreation.hpp"

#include "VulkanUtils.hpp"

#include "../DemoGame.hpp"

#include "renderPasses/GUIPass.hpp"
#include "renderPasses/ForwardPass.hpp"
#include "renderPasses/ShadowPass.hpp"
#include "renderPasses/OverlayPass.hpp"

#include "pipelineLayouts/ShadowPipelineLayout.hpp"
#include "pipelineLayouts/SkyboxPipelineLayout.hpp"
#include "pipelineLayouts/ForwardPipelineLayout.hpp"
#include "pipelineLayouts/GUIPipelineLayout.hpp"
#include "pipelineLayouts/DecalPipelineLayout.hpp"

#include "pipelines/ShadowPipeline.hpp"
#include "pipelines/SkyboxPipeline.hpp"
#include "pipelines/ForwardPipeline.hpp"
#include "pipelines/CrosshairPipeline.hpp"
#include "pipelines/DecalPipeline.hpp"

#include "textureBuffers/DepthTextureBuffer.hpp"
#include "textureBuffers/ShadowDepthTextureBuffer.hpp"
#include "textureBuffers/MultisampledColourTextureBuffer.hpp"
#include "textureBuffers/MultisampledDepthTextureBuffer.hpp"

#include "framebuffers/ShadowFramebuffer.hpp"
#include "framebuffers/ForwardFramebuffer.hpp"
#include "framebuffers/GUIFramebuffer.hpp"

#include "uniformBuffers/SceneUniformBuffer.hpp"
#include "uniformBuffers/DepthMVPUniformBuffer.hpp"
#include "uniformBuffers/OrthoUniformBuffer.hpp"

#include <GLFW/glfw3.h>

Renderer::Renderer(Engine::VulkanContext* context, Engine::EntityManager* entityManager, FPSTest* game) {
	this->context = context;
	this->entityManager = entityManager;
	this->game = game;

	std::size_t msaaOptionsSize = this->context->window->device->maxSampleCountIndex;
	for (std::size_t i = 0; i < msaaOptionsSize + 1; i++) {
		this->msaaOptions.push_back(this->context->window->device->msaaOptions[i]);
	}
}

void Renderer::initialise() {
	Engine::VulkanWindow* window = this->context->window.get();

	// Render Passes
	this->renderPasses.emplace("gui", std::make_unique<GUIPass>(window, &this->sampleCountSetting));
	this->renderPasses.emplace("shadow", std::make_unique<ShadowPass>(window, &this->sampleCountSetting));
	this->renderPasses.emplace("forward", std::make_unique<ForwardPass>(window, &this->sampleCountSetting));
	this->renderPasses.emplace("overlay", std::make_unique<OverlayPass>(window, &this->sampleCountSetting));

	// Descriptor Layouts
	std::vector<Engine::DescriptorSetting> sceneSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT} };
	this->descriptorLayouts.emplace("sceneLayout", Engine::createDescriptorLayout(*this->context->window, sceneSetting));

	std::vector<Engine::DescriptorSetting> vertSSBOSetting = { {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_VERTEX_BIT} };
	this->descriptorLayouts.emplace("vertSSBOLayout", Engine::createDescriptorLayout(*this->context->window, vertSSBOSetting));

	std::vector<Engine::DescriptorSetting> fragSSBOSetting = { {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT} };
	this->descriptorLayouts.emplace("fragSSBOLayout", Engine::createDescriptorLayout(*this->context->window, fragSSBOSetting));

	std::vector<Engine::DescriptorSetting> vertUBOSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT} };
	this->descriptorLayouts.emplace("vertUBOLayout", Engine::createDescriptorLayout(*this->context->window, vertUBOSetting));

	std::vector<Engine::DescriptorSetting> fragUBOSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT} };
	this->descriptorLayouts.emplace("fragUBOLayout", Engine::createDescriptorLayout(*this->context->window, fragUBOSetting));

	std::vector<Engine::DescriptorSetting> deferredSetting = { {VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT},
													{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT} ,
													{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT} ,
													{VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT, VK_SHADER_STAGE_FRAGMENT_BIT} };
	this->descriptorLayouts.emplace("deferredLayout", Engine::createDescriptorLayout(*this->context->window, deferredSetting));

	std::vector<Engine::DescriptorSetting> materialSetting = { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT}, // Base colour texture
													{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} ,   // Metalness/Roughness texture
													{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} ,   // Emissive texure
													{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} ,   // Occlusion texture
													{VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} };  // Normal texture
	this->descriptorLayouts.emplace("materialLayout", Engine::createDescriptorLayout(*this->context->window, materialSetting));

	std::vector<Engine::DescriptorSetting> fragImageSetting = { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} };
	this->descriptorLayouts.emplace("fragImageLayout", Engine::createDescriptorLayout(*this->context->window, fragImageSetting));

	std::vector<Engine::DescriptorSetting> orthoMatrices = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT} };
	this->descriptorLayouts.emplace("orthoMatrices", Engine::createDescriptorLayout(*this->context->window, orthoMatrices));

	// Pipeline Layouts
	this->pipelineLayouts.emplace("shadow", std::make_unique<ShadowPipelineLayout>(window, &this->descriptorLayouts));
	this->pipelineLayouts.emplace("skybox", std::make_unique<SkyboxPipelineLayout>(window, &this->descriptorLayouts));
	this->pipelineLayouts.emplace("forward", std::make_unique<ForwardPipelineLayout>(window, &this->descriptorLayouts, &this->shadowsEnabled));
	this->pipelineLayouts.emplace("gui", std::make_unique<GUIPipelineLayout>(window, &this->descriptorLayouts));
	this->pipelineLayouts.emplace("decal", std::make_unique<DecalPipelineLayout>(window, &this->descriptorLayouts));

	// Pipelines
	this->pipelines.emplace("shadow", std::make_unique<ShadowPipeline>(window, &this->pipelineLayouts["shadow"], this->renderPasses["shadow"].get(), &this->sampleCountSetting, &this->shadowResolution));
	this->pipelines.emplace("skybox", std::make_unique<SkyboxPipeline>(window, &this->pipelineLayouts["skybox"], this->renderPasses["forward"].get(), &this->sampleCountSetting));
	this->pipelines.emplace("forward", std::make_unique<ForwardPipeline>(window, &this->pipelineLayouts["forward"], this->renderPasses["forward"].get(), &this->sampleCountSetting, &this->shadowsEnabled));
	this->pipelines.emplace("crosshair", std::make_unique<CrosshairPipeline>(window, &this->pipelineLayouts["gui"], this->renderPasses["gui"].get(), &this->sampleCountSetting));
	this->pipelines.emplace("decal", std::make_unique<DecalPipeline>(window, &this->pipelineLayouts["decal"], this->renderPasses["forward"].get(), &this->sampleCountSetting));

	// Texture Buffers
	this->textureBuffers.emplace("depth", std::make_unique<DepthTextureBuffer>(this->context, &this->sampleCountSetting));
	this->textureBuffers.emplace("shadowDepth", std::make_unique<ShadowDepthTextureBuffer>(this->context, &this->sampleCountSetting, &this->shadowResolution));
	this->textureBuffers.emplace("multisampleColour", std::make_unique<MultisampledColourTextureBuffer>(this->context, &this->sampleCountSetting));
	this->textureBuffers.emplace("multisampleDepth", std::make_unique<MultisampledDepthTextureBuffer>(this->context, &this->sampleCountSetting));

	// Framebuffers
	this->framebuffers.emplace("shadow", std::make_unique<ShadowFramebuffer>(window, &this->textureBuffers, this->renderPasses["shadow"].get(), &this->shadowResolution));
	this->framebuffers.emplace("forward", std::make_unique<ForwardFramebuffer>(window, &this->textureBuffers, this->renderPasses["forward"].get(), &this->sampleCountSetting));
	this->framebuffers.emplace("gui", std::make_unique<GUIFramebuffer>(window, &this->textureBuffers, this->renderPasses["gui"].get()));

	// Setup synchronisation
	for (std::size_t i = 0; i < context->window->swapViews.size(); i++) {
		this->cmdBuffers.emplace_back(Engine::createCommandBuffer(*window));
		this->frameDoneFences.emplace_back(Engine::createFence(*window, VK_FENCE_CREATE_SIGNALED_BIT));
		this->imageAvailableSemaphores.emplace_back(Engine::createSemaphore(*window));
		this->renderFinishedSemaphores.emplace_back(Engine::createSemaphore(*window));
	}

	// Uniforms
	this->uniformBuffers.emplace("scene", std::make_unique<SceneUniformBuffer>(this->context->allocator.get(), &this->uniforms.sceneUniform));
	this->uniformBuffers.emplace("depthMVP", std::make_unique<DepthMVPUniformBuffer>(this->context->allocator.get(), &this->uniforms.depthMVP));
	this->uniformBuffers.emplace("orthoMatrices", std::make_unique<OrthoUniformBuffer>(this->context->allocator.get(), &this->uniforms.orthoMatrices));

	// Samplers
	Engine::vk::SamplerInfo samplerInfo = {
		VK_FILTER_LINEAR,
		VK_FILTER_LINEAR,
		VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
		1, VK_COMPARE_OP_LESS_OR_EQUAL };
	this->depthSampler = Engine::createTextureSampler(*window, samplerInfo);

	// Descriptor Sets
	this->descriptorSets.emplace("scene",
		Engine::createUBODescriptor(*window,
			this->descriptorLayouts["sceneLayout"].handle,
			this->uniformBuffers["scene"]->getBufferHandle()));
	this->descriptorSets.emplace("shadow",
		Engine::createUBODescriptor(*window,
			this->descriptorLayouts["vertUBOLayout"].handle,
			this->uniformBuffers["depthMVP"]->getBufferHandle()));
	this->descriptorSets.emplace("shadowMap",
		Engine::createImageDescriptor(*window,
			this->descriptorLayouts["fragImageLayout"].handle,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			this->textureBuffers["shadowDepth"]->getImageView().handle,
			this->depthSampler.handle));
	this->descriptorSets.emplace("orthoMatrices",
		Engine::createUBODescriptor(*window,
			this->descriptorLayouts["orthoMatrices"].handle,
			this->uniformBuffers["orthoMatrices"]->getBufferHandle()));
}

void Renderer::attachCamera(Engine::Camera* camera) {
	this->camera = camera;
}

void Renderer::initialiseJointMatrices() {
	std::vector<Engine::vk::Model>& models = this->game->GetModels();

	this->isSceneLoaded = true;

	std::vector<std::unique_ptr<Engine::ComponentBase>>* renderComponents = this->entityManager->GetComponentsOfType(Engine::RENDER);
	if (renderComponents == nullptr)
		return;

	for (std::size_t i = 0; i < renderComponents->size(); i++) {
		Engine::RenderComponent* renderComponent = reinterpret_cast<Engine::RenderComponent*>((*renderComponents)[i].get());
		int modelIndex = renderComponent->GetModelIndex();
		Engine::vk::Model& model = models[modelIndex];

		for (Engine::vk::Node* node : model.linearNodes) {
			if (!node->mesh)
				continue;

			node->descriptorBuffer = Engine::vk::createBuffer(
				"jointMatrix",
				*this->context->allocator,
				sizeof(glsl::SkinningUniform),
				VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
				0,
				VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
			node->descriptor = Engine::createUBODescriptor(*this->context->window, this->descriptorLayouts["vertUBOLayout"].handle, node->descriptorBuffer.buffer);
		}
	}
}

void Renderer::initialiseModelDescriptors() {
	std::vector<Engine::vk::Model>& models = this->game->GetModels();

	for (Engine::vk::Model& model : models)
		model.createDescriptorSets(*this->context, this->descriptorLayouts["materialLayout"].handle, this->descriptorLayouts["fragSSBOLayout"].handle);
}

void Renderer::addSkybox(std::unique_ptr<Engine::Skybox> skybox) {
	this->skybox = std::move(skybox);
}

bool Renderer::checkSwapchain() {
	if (!this->recreateSwapchain)
		return false;

	// Handle minimisation
	int width, height;
	glfwGetFramebufferSize(this->context->getGLFWWindow(), &width, &height);
	// Loop indefinitely until framebuffer size becomes non-zero (window is no longer minimised)
	while (width == 0 || height == 0) {
		glfwGetFramebufferSize(this->context->getGLFWWindow(), &width, &height);
		glfwWaitEvents();
	}

	// Wait on device
	vkDeviceWaitIdle(this->context->window->device->device);

	// Update MSAA setting
	this->sampleCountSetting = this->context->window->device->getSampleCount(this->msaaIndex);

	// Recreate swapchain stuff
	const auto changes = Engine::recreateSwapchain(*this->context->window, this->vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);

	this->recreateFormatDependents();
	this->recreateSizeDependents();
	this->recreateOthers();

	// Update crosshair positons
	this->game->GetCrosshair().shouldUpdateCrosshair = true;

	// Destroy and recreate all semaphores so none are in signaled state, which
	// may be the case when vkAcquireNextImageKHR returns VK_SUBOPTIMAL_KHR.
	this->imageAvailableSemaphores.clear();
	for (std::size_t i = 0; i < this->context->window->swapViews.size(); i++)
		this->imageAvailableSemaphores.emplace_back(Engine::createSemaphore(*this->context->window));

	this->recreateSwapchain = false;
	return true;
}

bool Renderer::acquireSwapchainImage() {
	this->frameIndex++;
	this->frameIndex %= this->cmdBuffers.size();

	Engine::waitForFences(*this->context->window, this->frameDoneFences, this->frameIndex);

	if (Engine::acquireNextSwapchainImage(*this->context->window, this->imageAvailableSemaphores, this->frameIndex, this->imageIndex)) {
		this->recreateSwapchain = true;

		--this->frameIndex;
		this->frameIndex %= this->cmdBuffers.size();

		return true;
	}

	Engine::resetFences(*this->context->window, this->frameDoneFences, this->frameIndex);

	return false;
}

void Renderer::updateAnimations(float timeDelta) {
	std::vector<Engine::vk::Model>& models = this->game->GetModels();

	std::vector<std::unique_ptr<Engine::ComponentBase>>* renderComponents = this->entityManager->GetComponentsOfType(Engine::RENDER);
	if (renderComponents == nullptr)
		return;

	for (std::size_t i = 0; i < renderComponents->size(); i++) {
		Engine::RenderComponent* renderComponent = reinterpret_cast<Engine::RenderComponent*>((*renderComponents)[i].get());
		int modelIndex = renderComponent->GetModelIndex();
		Engine::vk::Model& model = models[modelIndex];

		// Skip until we find any models with animations
		if (model.animations.size() == 0)
			continue;

		model.updateAnimation(timeDelta);
	}
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
	glm::mat4 depthProjection = glm::ortho(-20.0f, 20.0f, 20.0f, -20.0f, 0.1f, 1000.0f);
	glm::mat4 depthView = glm::lookAt(glm::vec3(0.75f, 20.0f, -0.4f), glm::vec3(0.75f, 0.0f, -0.4f), glm::vec3(1.0f, 0.0f, 0.0f));
	//glm::mat4 depthModel = glm::mat4(1.0f); // Model matrix is just identity

	this->uniforms.depthMVP.depthMVP = depthProjection * depthView;

	this->uniforms.orthoMatrices.projection = glm::ortho(0.0f, width, height, 0.0f);

	if (this->isSceneLoaded)
		this->updateModelMatrices();
}

void Renderer::updateModelMatrices() {
	std::vector<Engine::vk::Model>& models = this->game->GetModels();

	std::vector<int> renderEntities = this->entityManager->GetEntitiesWithComponent(Engine::RENDER);
	for (std::size_t i = 0; i < renderEntities.size(); i++) {
		Engine::RenderComponent* renderComponent = reinterpret_cast<Engine::RenderComponent*>(this->entityManager->GetComponentOfEntity(renderEntities[i], Engine::RENDER));

		if (!renderComponent->GetIsActive())
			continue;

		int modelIndex = renderComponent->GetModelIndex();
		Engine::vk::Model& model = models[modelIndex];

		for (std::size_t j = 0; j < model.linearNodes.size(); j++) {
			// Get the entities model matrix (post transform) and times it by the models model matrix (the one from the glTF file) 
			model.linearNodes[j]->globalMatrix = this->entityManager->GetEntity(renderEntities[i])->GetModelMatrix() * model.linearNodes[j]->getModelMatrix();
		}
	}
}

void Renderer::render() {
	RenderMode mode = this->game->getRenderMode();
	if (mode == NO_DATA_MODE)
		this->renderGUI();
	else if (mode == FORWARD)
		this->renderForward();
}

void Renderer::renderGUI() {
	// Get command buffer
	VkCommandBuffer cmdBuf = this->cmdBuffers[this->frameIndex];
	Engine::beginCommandBuffer(cmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	VkRenderPassBeginInfo passInfo{};
	passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passInfo.renderPass = this->renderPasses["gui"]->getRenderPassHandle();
	passInfo.framebuffer = this->framebuffers["gui"]->getFramebufferHandle(this->imageIndex);
	passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
	passInfo.renderArea.extent = this->context->window->swapchainExtent;
	passInfo.clearValueCount = static_cast<std::uint32_t>(this->renderPasses["gui"]->getClearValues().size());
	passInfo.pClearValues = this->renderPasses["gui"]->getClearValues().data();

	vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

	if (ImGui::GetDrawData() != nullptr)
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);

	vkCmdEndRenderPass(cmdBuf);

	Engine::endCommandBuffer(*this->context->window, cmdBuf);
}

void Renderer::renderForward() {
	std::vector<Engine::vk::Model>& models = this->game->GetModels();

	// Get command buffer
	VkCommandBuffer cmdBuf = this->cmdBuffers[this->frameIndex];
	Engine::beginCommandBuffer(cmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

	// Update uniforms
	this->uniformBuffers["scene"]->update(cmdBuf);

	std::vector<std::unique_ptr<Engine::ComponentBase>>* renderComponents = this->entityManager->GetComponentsOfType(Engine::RENDER);
	if (renderComponents == nullptr)
		return;

	for (std::size_t i = 0; i < renderComponents->size(); i++) {
		Engine::RenderComponent* renderComponent = reinterpret_cast<Engine::RenderComponent*>((*renderComponents)[i].get());

		int modelIndex = renderComponent->GetModelIndex();
		Engine::vk::Model& model = models[modelIndex];

		for (Engine::vk::Node* node : model.linearNodes) {
			if (!node->mesh)
				continue;

			// Just do regular uniform buffer updating for these for now
			Utils::bufferBarrier(
				cmdBuf,
				node->descriptorBuffer.buffer,
				VK_ACCESS_UNIFORM_READ_BIT,
				VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT);

			vkCmdUpdateBuffer(cmdBuf, node->descriptorBuffer.buffer, 0, sizeof(glsl::SkinningUniform), &node->skinUniform);

			Utils::bufferBarrier(
				cmdBuf,
				node->descriptorBuffer.buffer,
				VK_ACCESS_TRANSFER_WRITE_BIT,
				VK_ACCESS_UNIFORM_READ_BIT,
				VK_PIPELINE_STAGE_TRANSFER_BIT,
				VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);
		}
	}

	this->game->getDecals().updateUniform(cmdBuf);

	// If shadows enabled, do shadow pass
	if (this->shadowsEnabled) {
		this->uniformBuffers["depthMVP"]->update(cmdBuf);

		VkRenderPassBeginInfo passInfo{};
		passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passInfo.renderPass = this->renderPasses["shadow"]->getRenderPassHandle();
		passInfo.framebuffer = this->framebuffers["shadow"]->getFramebufferHandle(this->imageIndex);
		passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo.renderArea.extent = this->shadowResolution;
		passInfo.clearValueCount = static_cast<std::uint32_t>(this->renderPasses["shadow"]->getClearValues().size());
		passInfo.pClearValues = this->renderPasses["shadow"]->getClearValues().data();

		vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["shadow"]->getPipelineHandle());
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["shadow"]->getPipelineLayoutHandle(), 0, 1, &this->descriptorSets["shadow"], 0, nullptr);
		vkCmdSetDepthBias(cmdBuf, this->depthBiasConstant, 0.0f, this->depthBiasSlopeFactor);

		this->drawModels(cmdBuf, this->pipelineLayouts["shadow"]->getPipelineLayoutHandle(), Engine::DrawType::WORLD, true);

		vkCmdEndRenderPass(cmdBuf);
	}

	VkRenderPassBeginInfo passInfo{};
	passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passInfo.renderPass = this->renderPasses["forward"]->getRenderPassHandle();
	passInfo.framebuffer = this->framebuffers["forward"]->getFramebufferHandle(this->imageIndex);
	passInfo.renderArea.offset = VkOffset2D { 0, 0 };
	passInfo.renderArea.extent = this->context->window->swapchainExtent;
	passInfo.clearValueCount = static_cast<std::uint32_t>(this->renderPasses["forward"]->getClearValues().size());
	passInfo.pClearValues = this->renderPasses["forward"]->getClearValues().data();

	vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

	// Render skybox
	if (this->skybox) {
		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["skybox"]->getPipelineHandle());
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["skybox"]->getPipelineLayoutHandle(), 0, 1, &this->descriptorSets["scene"], 0, nullptr);
		this->skybox->bind(cmdBuf, this->pipelineLayouts["skybox"]->getPipelineLayoutHandle());

		vkCmdDraw(cmdBuf, 36, 1, 0, 0);
	}

	// Render models
	vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["forward"]->getPipelineHandle());
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"]->getPipelineLayoutHandle(), 0, 1, &this->descriptorSets["scene"], 0, nullptr);

	if (this->shadowsEnabled) {
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"]->getPipelineLayoutHandle(), 4, 1, &this->descriptorSets["shadow"], 0, nullptr); // Depth matrix
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"]->getPipelineLayoutHandle(), 5, 1, &this->descriptorSets["shadowMap"], 0, nullptr); // Shadow map
	}

	this->drawModels(cmdBuf, this->pipelineLayouts["forward"]->getPipelineLayoutHandle(), Engine::DrawType::WORLD);

	// Render decals
	vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["decal"]->getPipelineHandle());

	VkDescriptorSet decalTransformDescriptorSet = this->game->getDecals().getTransformDescriptorSet();
	VkDescriptorSet decalImageDescriptorSet = this->game->getDecals().getImageDescriptorSet();
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["decal"]->getPipelineLayoutHandle(), 0, 1, &this->descriptorSets["scene"], 0, nullptr); // Projective matrices
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["decal"]->getPipelineLayoutHandle(), 1, 1, &decalTransformDescriptorSet, 0, nullptr);
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["decal"]->getPipelineLayoutHandle(), 2, 1, &decalImageDescriptorSet, 0, nullptr);

	this->game->getDecals().render(cmdBuf);

	vkCmdEndRenderPass(cmdBuf);

	// Render overlays

	// By setting the view matrix to the identity matrix we skip
	// the transformation into camera space and let the projection
	// matrix put it straight into screen space
	this->uniforms.sceneUniform.view = glm::mat4(1.0f);

	this->uniformBuffers["scene"]->update(cmdBuf);

	VkRenderPassBeginInfo passInfo2{};
	passInfo2.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passInfo2.renderPass = this->renderPasses["overlay"]->getRenderPassHandle();
	passInfo2.framebuffer = this->framebuffers["forward"]->getFramebufferHandle(this->imageIndex);
	passInfo2.renderArea.offset = VkOffset2D{ 0, 0 };
	passInfo2.renderArea.extent = this->context->window->swapchainExtent;
	passInfo2.clearValueCount = static_cast<uint32_t>(this->renderPasses["overlay"]->getClearValues().size());
	passInfo2.pClearValues = this->renderPasses["overlay"]->getClearValues().data();

	vkCmdBeginRenderPass(cmdBuf, &passInfo2, VK_SUBPASS_CONTENTS_INLINE);

	vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["forward"]->getPipelineHandle());
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"]->getPipelineLayoutHandle(), 0, 1, &this->descriptorSets["scene"], 0, nullptr);

	if (this->shadowsEnabled) {
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"]->getPipelineLayoutHandle(), 4, 1, &this->descriptorSets["shadow"], 0, nullptr); // Depth matrix
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"]->getPipelineLayoutHandle(), 5, 1, &this->descriptorSets["shadowMap"], 0, nullptr); // Shadow map
	}

	this->drawModels(cmdBuf, this->pipelineLayouts["forward"]->getPipelineLayoutHandle(), Engine::DrawType::OVERLAY);

	vkCmdEndRenderPass(cmdBuf);

	// Draw crosshair and GUI
	this->game->GetCrosshair().updatePositions();
	this->uniformBuffers["orthoMatrices"]->update(cmdBuf);

	VkRenderPassBeginInfo passInfo3{};
	passInfo3.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
	passInfo3.renderPass = this->renderPasses["gui"]->getRenderPassHandle();
	passInfo3.framebuffer = this->framebuffers["gui"]->getFramebufferHandle(this->imageIndex);
	passInfo3.renderArea.offset = VkOffset2D{ 0, 0 };
	passInfo3.renderArea.extent = this->context->window->swapchainExtent;
	passInfo3.clearValueCount = static_cast<uint32_t>(this->renderPasses["gui"]->getClearValues().size());
	passInfo3.pClearValues = this->renderPasses["gui"]->getClearValues().data();

	vkCmdBeginRenderPass(cmdBuf, &passInfo3, VK_SUBPASS_CONTENTS_INLINE);

	// Due to the nature of the GUI pass and framebuffer we can just render the crosshair here too
	vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["crosshair"]->getPipelineHandle());
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["gui"]->getPipelineLayoutHandle(), 0, 1, &this->descriptorSets["orthoMatrices"], 0, nullptr);

	this->game->GetCrosshair().drawCrosshair(cmdBuf);

	if (ImGui::GetDrawData() != nullptr)
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);

	vkCmdEndRenderPass(cmdBuf);

	Engine::endCommandBuffer(*this->context->window, cmdBuf);
}

void Renderer::submitRender() {
	const VkResult result = Engine::submitAndPresent(
		*this->context->window,
		this->cmdBuffers,
		this->frameDoneFences,
		this->imageAvailableSemaphores,
		this->renderFinishedSemaphores,
		this->frameIndex,
		this->imageIndex);

	if (VK_SUBOPTIMAL_KHR == result || VK_ERROR_OUT_OF_DATE_KHR == result)
		this->recreateSwapchain = true;
	else if (VK_SUCCESS != result)
		throw Utils::Error("Unable to present swapchain image %u\n vkQueuePresentKHR() returned %s", this->imageIndex, Utils::toString(result).c_str());
}

void Renderer::finishRendering() {
	vkDeviceWaitIdle(this->context->window->device->device);

	this->destroyImGui();
}

void Renderer::unloadScene() {
	this->isSceneLoaded = false;
}

void Renderer::destroyImGui() {
	ImGui_ImplVulkan_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();
}

void Renderer::drawModels(VkCommandBuffer cmdBuf, VkPipelineLayout pipelineLayout, Engine::DrawType drawType, bool justGeometry) {
	std::vector<Engine::vk::Model>& models = this->game->GetModels();

	std::vector<std::unique_ptr<Engine::ComponentBase>>* renderComponents = this->entityManager->GetComponentsOfType(Engine::RENDER);
	if (renderComponents == nullptr)
		return;

	for (std::size_t i = 0; i < renderComponents->size(); i++) {
		Engine::RenderComponent* renderComponent = reinterpret_cast<Engine::RenderComponent*>((*renderComponents)[i].get());
		
		if (!renderComponent->GetIsActive())
			continue;

		int modelIndex = renderComponent->GetModelIndex();

		if (models[modelIndex].drawType != drawType) continue;

		models[modelIndex].drawModel(cmdBuf, pipelineLayout, justGeometry);
	}
}

void Renderer::calculateFPS() {
	float currTime = glfwGetTime();

	this->frameTime = currTime - this->prevTime;
	this->frames++;
	this->frameTimes.push_back(frameTime);
	if (currTime - this->lastSecondTime >= 1.0f) {
		this->avgFrameTime = std::reduce(this->frameTimes.begin(), this->frameTimes.end()) / this->frames;
		this->avgFps = 1 / this->avgFrameTime;
		this->frameTimes.clear();
		this->frames = 0;
		this->lastSecondTime = currTime;

		// Put fps and frame time in window title so we can see them in both debug and release
		std::ostringstream oss;
		oss << "FPS Test Game - Avg FPS: " << this->avgFps << " - Avg Frame Time: " << std::fixed << std::setprecision(3) << this->avgFrameTime * 1000.0f << "ms";
		std::string title = oss.str();
		glfwSetWindowTitle(this->context->getGLFWWindow(), title.c_str());
	}
	this->prevTime = currTime;
}

void Renderer::setRecreateSwapchain(bool value) {
	this->recreateSwapchain = value;
}

VkRenderPass Renderer::getRenderPassHandle(const std::string& renderPass) {
	return this->renderPasses[renderPass]->getRenderPassHandle();
}

Engine::Camera* Renderer::getCameraPointer() {
	return this->camera;
}

bool Renderer::getIsSceneLoaded() {
	return this->isSceneLoaded;
}

int& Renderer::getMSAAIndex() {
	return this->msaaIndex;
}

float Renderer::getAvgFrameTime() {
	return this->avgFrameTime;
}

int Renderer::getAvgFPS() {
	return this->avgFps;
}

bool& Renderer::getShadowState() {
	return this->shadowsEnabled;
}

VkDescriptorSetLayout Renderer::getDescriptorLayout(const std::string& handle) {
	return this->descriptorLayouts[handle].handle;
}

std::pair<const char**, int> Renderer::getMSAAOptions() {
	return { this->msaaOptions.data(), (int)this->msaaOptions.size() };
}

void Renderer::recreateFormatDependents() {
	// Recreate render passes
	for (auto& renderPass : this->renderPasses) {
		renderPass.second->recreate();
	}
}

void Renderer::recreateSizeDependents() {
	// Recreate texture buffers
	for (auto& textureBuffer : this->textureBuffers) {
		textureBuffer.second->recreate();
	}

	// Recreate pipeline layouts
	for (auto& pipelineLayout : this->pipelineLayouts) {
		pipelineLayout.second->recreate();
	}

	// Recreate pipelines
	for (auto& pipeline : this->pipelines) {
		pipeline.second->recreate();
	}
}

void Renderer::recreateOthers() {
	// Recreate framebuffers
	for (auto& framebuffer : this->framebuffers) {
		framebuffer.second->recreate();
	}

	// Recreate descriptor sets
	this->descriptorSets["shadowMap"] = 
		Engine::createImageDescriptor(*this->context->window,
			this->descriptorLayouts["fragImageLayout"].handle,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			this->textureBuffers["shadowDepth"]->getImageView().handle,
			this->depthSampler.handle);
}
#include "Renderer.hpp"

#include <numeric>
#include <iomanip>

#include "Engine/vulkan/VulkanContext.hpp"
#include "Engine/vulkan/VulkanDevice.hpp"
#include "Engine/Rendering/PipelineCreation.hpp"
#include "Engine/Rendering/RenderingUtils.hpp"

#include "VulkanUtils.hpp"

#include "../DemoGame.hpp"

#include "Engine/Rendering/objects/defaults/renderPasses/GUIPass.hpp"
#include "Engine/Rendering/objects/defaults/renderPasses/ForwardPass.hpp"
#include "Engine/Rendering/objects/defaults/renderPasses/ShadowPass.hpp"
#include "Engine/Rendering/objects/defaults/renderPasses/OverlayPass.hpp"

#include "Engine/Rendering/objects/defaults/pipelineLayouts/ShadowPipelineLayout.hpp"
#include "Engine/Rendering/objects/defaults/pipelineLayouts/SkyboxPipelineLayout.hpp"
#include "Engine/Rendering/objects/defaults/pipelineLayouts/ForwardPipelineLayout.hpp"
#include "Engine/Rendering/objects/defaults/pipelineLayouts/GUIPipelineLayout.hpp"
#include "Engine/Rendering/objects/defaults/pipelineLayouts/DecalPipelineLayout.hpp"

#include "Engine/Rendering/objects/defaults/pipelines/ShadowPipeline.hpp"
#include "Engine/Rendering/objects/defaults/pipelines/SkyboxPipeline.hpp"
#include "Engine/Rendering/objects/defaults/pipelines/ForwardPipeline.hpp"
#include "Engine/Rendering/objects/defaults/pipelines/DecalPipeline.hpp"
#include "pipelines/CrosshairPipeline.hpp"

#include "Engine/Rendering/objects/defaults/textureBuffers/DepthTextureBuffer.hpp"
#include "Engine/Rendering/objects/defaults/textureBuffers/ShadowDepthTextureBuffer.hpp"
#include "Engine/Rendering/objects/defaults/textureBuffers/MultisampledColourTextureBuffer.hpp"
#include "Engine/Rendering/objects/defaults/textureBuffers/MultisampledDepthTextureBuffer.hpp"

#include "Engine/Rendering/objects/defaults/framebuffers/ShadowFramebuffer.hpp"
#include "Engine/Rendering/objects/defaults/framebuffers/ForwardFramebuffer.hpp"
#include "Engine/Rendering/objects/defaults/framebuffers/GUIFramebuffer.hpp"

#include "Engine/Rendering/objects/defaults/uniformBuffers/SceneUniformBuffer.hpp"
#include "Engine/Rendering/objects/defaults/uniformBuffers/DepthMVPUniformBuffer.hpp"
#include "Engine/Rendering/objects/defaults/uniformBuffers/OrthoUniformBuffer.hpp"

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
	this->pipelines.emplace("shadow", std::make_unique<ShadowPipeline>(window, &this->pipelineLayouts["shadow"], this->renderPasses["shadow"].get(), &this->sampleCountSetting, &this->currentShadowResolution));
	this->pipelines.emplace("skybox", std::make_unique<SkyboxPipeline>(window, &this->pipelineLayouts["skybox"], this->renderPasses["forward"].get(), &this->sampleCountSetting));
	this->pipelines.emplace("forward", std::make_unique<ForwardPipeline>(window, &this->pipelineLayouts["forward"], this->renderPasses["forward"].get(), &this->sampleCountSetting, &this->shadowsEnabled));
	this->pipelines.emplace("crosshair", std::make_unique<CrosshairPipeline>(window, &this->pipelineLayouts["gui"], this->renderPasses["gui"].get(), &this->sampleCountSetting));
	this->pipelines.emplace("decal", std::make_unique<DecalPipeline>(window, &this->pipelineLayouts["decal"], this->renderPasses["forward"].get(), &this->sampleCountSetting));

	// Texture Buffers
	this->textureBuffers.emplace("depth", std::make_unique<DepthTextureBuffer>(this->context, &this->sampleCountSetting));
	this->textureBuffers.emplace("shadowDepth", std::make_unique<ShadowDepthTextureBuffer>(this->context, &this->sampleCountSetting, &this->currentShadowResolution));
	this->textureBuffers.emplace("multisampleColour", std::make_unique<MultisampledColourTextureBuffer>(this->context, &this->sampleCountSetting));
	this->textureBuffers.emplace("multisampleDepth", std::make_unique<MultisampledDepthTextureBuffer>(this->context, &this->sampleCountSetting));

	// Framebuffers
	this->framebuffers.emplace("shadow", std::make_unique<ShadowFramebuffer>(window, &this->textureBuffers, this->renderPasses["shadow"].get(), &this->currentShadowResolution));
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
			this->uniformBuffers["scene"]->getHandle()));
	this->descriptorSets.emplace("shadow",
		Engine::createUBODescriptor(*window,
			this->descriptorLayouts["vertUBOLayout"].handle,
			this->uniformBuffers["depthMVP"]->getHandle()));
	this->descriptorSets.emplace("shadowMap",
		Engine::createImageDescriptor(*window,
			this->descriptorLayouts["fragImageLayout"].handle,
			VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL,
			this->textureBuffers["shadowDepth"]->getImageView().handle,
			this->depthSampler.handle));
	this->descriptorSets.emplace("orthoMatrices",
		Engine::createUBODescriptor(*window,
			this->descriptorLayouts["orthoMatrices"].handle,
			this->uniformBuffers["orthoMatrices"]->getHandle()));
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
	glm::mat4 depthProjection = glm::ortho(-30.0f, 30.0f, 30.0f, -30.0f, 0.1f, 1000.0f);
	glm::mat4 depthView = glm::lookAt(glm::vec3(20.0f, 20.0f, -40.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::mat4 depthModel = glm::mat4(1.0f); // Model matrix is just identity

	this->uniforms.depthMVP.depthMVP = depthProjection * depthView;

	this->uniforms.orthoMatrices.projection = glm::ortho(0.0f, width, height, 0.0f);
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

	// Since the GUI pass expects an image already in VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
	// and if it is the first render pass then the swapchain image will be in 
	// VK_IMAGE_LAYOUT_UNDEFINED, so we need to transition it along with a clear color image cmd.
	// Transition to OPTIMAL
	Utils::imageBarrier(cmdBuf,
		this->context->window->swapImages[this->imageIndex],
		0, 0,
		VK_IMAGE_LAYOUT_UNDEFINED,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT);

	VkClearColorValue clearColor = { {0.0f, 0.0f, 0.0f, 1.0f} }; // Black with full alpha
	VkImageSubresourceRange clearRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

	vkCmdClearColorImage(cmdBuf,
		this->context->window->swapImages[this->imageIndex],
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, &clearColor, 1, &clearRange);

	// Transition to PRESENT
	Utils::imageBarrier(cmdBuf,
		this->context->window->swapImages[this->imageIndex],
		0, 0,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_PRESENT_SRC_KHR,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT);

	Engine::beginRenderPass(cmdBuf, this->renderPasses["gui"].get(), this->framebuffers["gui"].get(), this->imageIndex);

	if (ImGui::GetDrawData() != nullptr)
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);

	Engine::endRenderPass(cmdBuf);
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

	this->game->getBulletDecals().updateUniform(cmdBuf);

	// If shadows enabled, do shadow pass
	if (this->shadowsEnabled) {
		this->uniformBuffers["depthMVP"]->update(cmdBuf);

		Engine::beginRenderPass(cmdBuf, this->renderPasses["shadow"].get(), this->framebuffers["shadow"].get(), this->imageIndex);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["shadow"]->getHandle());
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["shadow"]->getHandle(), 0, 1, &this->descriptorSets["shadow"], 0, nullptr);
		vkCmdSetDepthBias(cmdBuf, this->depthBiasConstant, 0.0f, this->depthBiasSlopeFactor);

		this->drawModels(cmdBuf, this->pipelineLayouts["shadow"]->getHandle(), Engine::DrawType::WORLD, true);

		Engine::endRenderPass(cmdBuf);
	}

	Engine::beginRenderPass(cmdBuf, this->renderPasses["forward"].get(), this->framebuffers["forward"].get(), this->imageIndex);

	// Render skybox
	if (this->skybox) {
		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["skybox"]->getHandle());
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["skybox"]->getHandle(), 0, 1, &this->descriptorSets["scene"], 0, nullptr);
		this->skybox->bind(cmdBuf, this->pipelineLayouts["skybox"]->getHandle());

		vkCmdDraw(cmdBuf, 36, 1, 0, 0);
	}

	// Render models
	vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["forward"]->getHandle());
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"]->getHandle(), 0, 1, &this->descriptorSets["scene"], 0, nullptr);

	if (this->shadowsEnabled) {
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"]->getHandle(), 4, 1, &this->descriptorSets["shadow"], 0, nullptr); // Depth matrix
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"]->getHandle(), 5, 1, &this->descriptorSets["shadowMap"], 0, nullptr); // Shadow map
	}

	this->drawModels(cmdBuf, this->pipelineLayouts["forward"]->getHandle(), Engine::DrawType::WORLD);

	// Render decals
	vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["decal"]->getHandle());

	VkDescriptorSet decalTransformDescriptorSet = this->game->getBulletDecals().getTransformDescriptorSet();
	VkDescriptorSet decalImageDescriptorSet = this->game->getBulletDecals().getImageDescriptorSet();
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["decal"]->getHandle(), 0, 1, &this->descriptorSets["scene"], 0, nullptr); // Projective matrices
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["decal"]->getHandle(), 1, 1, &decalTransformDescriptorSet, 0, nullptr);
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["decal"]->getHandle(), 2, 1, &decalImageDescriptorSet, 0, nullptr);

	this->game->getBulletDecals().render(cmdBuf);

	Engine::endRenderPass(cmdBuf);

	// Render overlays

	// By setting the view matrix to the identity matrix we skip
	// the transformation into camera space and let the projection
	// matrix put it straight into screen space
	this->uniforms.sceneUniform.view = glm::mat4(1.0f);

	this->uniformBuffers["scene"]->update(cmdBuf);

	Engine::beginRenderPass(cmdBuf, this->renderPasses["overlay"].get(), this->framebuffers["forward"].get(), this->imageIndex);

	vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["forward"]->getHandle());
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"]->getHandle(), 0, 1, &this->descriptorSets["scene"], 0, nullptr);

	if (this->shadowsEnabled) {
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"]->getHandle(), 4, 1, &this->descriptorSets["shadow"], 0, nullptr); // Depth matrix
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"]->getHandle(), 5, 1, &this->descriptorSets["shadowMap"], 0, nullptr); // Shadow map
	}

	this->drawModels(cmdBuf, this->pipelineLayouts["forward"]->getHandle(), Engine::DrawType::OVERLAY);

	Engine::endRenderPass(cmdBuf);

	// Draw crosshair and GUI
	this->game->GetCrosshair().updatePositions();
	this->uniformBuffers["orthoMatrices"]->update(cmdBuf);

	Engine::beginRenderPass(cmdBuf, this->renderPasses["gui"].get(), this->framebuffers["gui"].get(), this->imageIndex);

	// Due to the nature of the GUI pass and framebuffer we can just render the crosshair here too
	vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["crosshair"]->getHandle());
	vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["gui"]->getHandle(), 0, 1, &this->descriptorSets["orthoMatrices"], 0, nullptr);

	this->game->GetCrosshair().drawCrosshair(cmdBuf);

	if (ImGui::GetDrawData() != nullptr)
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);

	Engine::endRenderPass(cmdBuf);

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

		// if (game->GetGameMode().IsMultiPlayer() && game->GetGameMode().GetPlayerEntity() == renderComponent->GetEntityPointer())
		// 	continue;

		int modelIndex = renderComponent->GetModelIndex();

		if (models[modelIndex].drawType != drawType) continue;

		models[modelIndex].drawModel(cmdBuf, pipelineLayout, renderComponent->GetEntity(), justGeometry);
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

void Renderer::updateShadowMapResolution() {
	this->currentShadowResolution = this->shadowResolutions[this->shadowResolutionIndex];
}

VkRenderPass Renderer::getRenderPassHandle(const std::string& renderPass) {
	return this->renderPasses[renderPass]->getHandle();
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

int& Renderer::getShadowResolutionIndex() {
	return this->shadowResolutionIndex;
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
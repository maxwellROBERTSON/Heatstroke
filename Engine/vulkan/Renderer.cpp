#include "Renderer.hpp"

#include <format>
#include <numeric>

#include "../ECS/Components/RenderComponent.hpp"
#include "Error.hpp"
#include "PipelineCreation.hpp"
#include "Skybox.hpp"
#include "toString.hpp"
#include "VulkanDevice.hpp"
#include "VulkanUtils.hpp"

#include "../../Game/DemoGame.hpp"

#include "Utils.hpp"
#include "vulkan/vulkan_core.h"

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.cpp>
#include <backends/imgui_impl_vulkan.h>
#include <GLFW\glfw3.h>
#include <imgui.h>

#define MAX_JOINTS 128u

namespace Engine {

	Renderer::Renderer(VulkanContext* aContext, EntityManager* entityManager, Game* game) {
		this->context = aContext;
		this->entityManager = entityManager;
		this->game = game;

		std::size_t msaaOptionsSize = this->context->window->device->maxSampleCountIndex;
		for (std::size_t i = 0; i < msaaOptionsSize + 1; i++) {
			this->msaaOptions.push_back(this->context->window->device->msaaOptions[i]);
		}
	}

	VkRenderPass& Renderer::GetRenderPass(std::string s) {
		return this->renderPasses[s].handle;
	}

	void Renderer::addSkybox(std::unique_ptr<Skybox> skybox) {
		this->skybox = std::move(skybox);
	}

	void Renderer::initialiseRenderer() {
		// Render passes
		this->renderPasses.emplace("forward", createRenderPass(*this->context->window));
		// Use 4 bit sample initially since Vulkan spec states implementations must support at least 1 and 4 bit sample counts.
		this->renderPasses.emplace("forwardMSAA", createRenderPassMSAA(*this->context->window, VK_SAMPLE_COUNT_4_BIT));
		this->renderPasses.emplace("deferred", createDeferredRenderPass(*this->context->window));
		this->renderPasses.emplace("shadow", createShadowRenderPass(*this->context->window));
		this->renderPasses.emplace("ui", createUIRenderPass(*this->context->window));
		this->renderPasses.emplace("crosshair", createCrosshairRenderPass(*this->context->window));

		// Descriptor Layouts
		std::vector<DescriptorSetting> sceneSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("sceneLayout", createDescriptorLayout(*this->context->window, sceneSetting));

		std::vector<DescriptorSetting> ssboSetting = { {VK_DESCRIPTOR_TYPE_STORAGE_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("SSBOLayout", createDescriptorLayout(*this->context->window, ssboSetting));

		std::vector<DescriptorSetting> vertUBOSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT} };
		this->descriptorLayouts.emplace("vertUBOLayout", createDescriptorLayout(*this->context->window, vertUBOSetting));

		std::vector<DescriptorSetting> fragUBOSetting = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("fragUBOLayout", createDescriptorLayout(*this->context->window, fragUBOSetting));

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

		std::vector<DescriptorSetting> fragImageSetting = { {VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, VK_SHADER_STAGE_FRAGMENT_BIT} };
		this->descriptorLayouts.emplace("fragImageLayout", createDescriptorLayout(*this->context->window, fragImageSetting));

		std::vector<DescriptorSetting> orthoMatrices = { {VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_VERTEX_BIT} };
		this->descriptorLayouts.emplace("orthoMatrices", createDescriptorLayout(*this->context->window, orthoMatrices));

		// Descriptor Layout groups (groups descriptor layouts for pipelines)

		std::vector<VkDescriptorSetLayout> forwardLayouts;
		forwardLayouts.emplace_back(this->descriptorLayouts["sceneLayout"].handle);
		forwardLayouts.emplace_back(this->descriptorLayouts["vertUBOLayout"].handle);
		forwardLayouts.emplace_back(this->descriptorLayouts["materialLayout"].handle);
		forwardLayouts.emplace_back(this->descriptorLayouts["SSBOLayout"].handle);

		std::vector<VkDescriptorSetLayout> forwardShadowLayouts;
		forwardShadowLayouts.emplace_back(this->descriptorLayouts["sceneLayout"].handle);
		forwardShadowLayouts.emplace_back(this->descriptorLayouts["vertUBOLayout"].handle);
		forwardShadowLayouts.emplace_back(this->descriptorLayouts["materialLayout"].handle);
		forwardShadowLayouts.emplace_back(this->descriptorLayouts["SSBOLayout"].handle);
		forwardShadowLayouts.emplace_back(this->descriptorLayouts["vertUBOLayout"].handle);
		forwardShadowLayouts.emplace_back(this->descriptorLayouts["fragImageLayout"].handle);

		std::vector<VkDescriptorSetLayout> deferredShadingLayouts;
		deferredShadingLayouts.emplace_back(this->descriptorLayouts["deferredLayout"].handle);
		deferredShadingLayouts.emplace_back(this->descriptorLayouts["sceneLayout"].handle);
		deferredShadingLayouts.emplace_back(this->descriptorLayouts["fragUBOLayout"].handle);

		std::vector<VkDescriptorSetLayout> shadowLayout;
		shadowLayout.emplace_back(this->descriptorLayouts["vertUBOLayout"].handle);
		shadowLayout.emplace_back(this->descriptorLayouts["vertUBOLayout"].handle);

		std::vector<VkDescriptorSetLayout> skyboxLayout;
		skyboxLayout.emplace_back(this->descriptorLayouts["sceneLayout"].handle);
		skyboxLayout.emplace_back(this->descriptorLayouts["fragImageLayout"].handle);

		std::vector<VkDescriptorSetLayout> crosshairLayout;
		crosshairLayout.emplace_back(this->descriptorLayouts["orthoMatrices"].handle);

		// Pipeline layouts
		// Empty push constant range
		std::vector<VkPushConstantRange> emptyPushConstant;
		
		VkPushConstantRange modelMatrixPushConstant = {
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
			.size = sizeof(glm::mat4)
		};

		VkPushConstantRange materialIndexPushConstant = {
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT,
			.offset = sizeof(glm::mat4), // (this assumes it will be put with a push constant of size glm::mat4 before this push constant)
			.size = sizeof(std::uint32_t)
		};

		// ModelPushConstant
		std::vector<VkPushConstantRange> modelMatrixAndMatIdxPushConstant;
		modelMatrixAndMatIdxPushConstant.emplace_back(modelMatrixPushConstant);
		modelMatrixAndMatIdxPushConstant.emplace_back(materialIndexPushConstant);

		std::vector<VkPushConstantRange> justModelMatrix;
		justModelMatrix.emplace_back(modelMatrixPushConstant);

		this->pipelineLayouts.emplace("forward", createPipelineLayout(*this->context->window, forwardLayouts, modelMatrixAndMatIdxPushConstant));
		this->pipelineLayouts.emplace("forwardShadow", createPipelineLayout(*this->context->window, forwardShadowLayouts, modelMatrixAndMatIdxPushConstant));
		this->pipelineLayouts.emplace("deferred", createPipelineLayout(*this->context->window, deferredShadingLayouts, emptyPushConstant));
		this->pipelineLayouts.emplace("shadow", createPipelineLayout(*this->context->window, shadowLayout, justModelMatrix));
		this->pipelineLayouts.emplace("skybox", createPipelineLayout(*this->context->window, skyboxLayout, emptyPushConstant));
		this->pipelineLayouts.emplace("crosshair", createPipelineLayout(*this->context->window, crosshairLayout, emptyPushConstant));

		// Pipelines
		std::tuple<vk::Pipeline, vk::Pipeline> deferredPipelines = createDeferredPipelines(
			*this->context->window,
			this->renderPasses["deferred"].handle,
			this->pipelineLayouts["forward"].handle,
			this->pipelineLayouts["deferred"].handle);

		this->pipelines.emplace("forward", createForwardPipeline(*this->context->window, this->renderPasses["forward"].handle, this->pipelineLayouts["forward"].handle, false));
		this->pipelines.emplace("forwardMSAA", createForwardPipeline(*this->context->window, this->renderPasses["forwardMSAA"].handle, this->pipelineLayouts["forward"].handle, false, VK_SAMPLE_COUNT_4_BIT));
		this->pipelines.emplace("forwardShadow", createForwardPipeline(*this->context->window, this->renderPasses["forward"].handle, this->pipelineLayouts["forwardShadow"].handle, true));
		this->pipelines.emplace("forwardShadowMSAA", createForwardPipeline(*this->context->window, this->renderPasses["forwardMSAA"].handle, this->pipelineLayouts["forwardShadow"].handle, true, VK_SAMPLE_COUNT_4_BIT));
		this->pipelines.emplace("gBufWrite", std::move(std::get<0>(deferredPipelines)));
		this->pipelines.emplace("deferred", std::move(std::get<1>(deferredPipelines)));
		this->pipelines.emplace("shadow", createShadowOffscreenPipeline(*this->context->window, this->renderPasses["shadow"].handle, this->pipelineLayouts["shadow"].handle));
		this->pipelines.emplace("skybox", createSkyboxPipeline(*this->context->window, this->renderPasses["forward"].handle, this->pipelineLayouts["skybox"].handle));
		this->pipelines.emplace("skyboxMSAA", createSkyboxPipeline(*this->context->window, this->renderPasses["forwardMSAA"].handle, this->pipelineLayouts["skybox"].handle, VK_SAMPLE_COUNT_4_BIT));
		this->pipelines.emplace("crosshair", createCrosshairPipeline(*this->context->window, this->renderPasses["crosshair"].handle, this->pipelineLayouts["crosshair"].handle));

		// Buffers
		TextureBufferSetting depthTexture = {
			.imageFormat = VK_FORMAT_D32_SFLOAT_S8_UINT,
			.imageExtent = this->context->window->swapchainExtent,
			.imageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT };
		this->buffers.emplace("depth", createTextureBuffer(*this->context, depthTexture));

		TextureBufferSetting colorTexture = {
			.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT,
			.imageExtent = this->context->window->swapchainExtent,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			.viewAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT };
		this->buffers.emplace("normals", createTextureBuffer(*this->context, colorTexture));
		this->buffers.emplace("albedo", createTextureBuffer(*this->context, colorTexture));
		this->buffers.emplace("emissive", createTextureBuffer(*this->context, colorTexture));

		TextureBufferSetting shadowDepthTexture = {
			.imageFormat = VK_FORMAT_D32_SFLOAT_S8_UINT,
			.imageExtent = VkExtent2D {2048, 2048}, // Probably will want to make the shadow map resolution more easily editable, rather than hardcoding it everywhere
			.imageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT };
		this->buffers.emplace("shadowDepth", createTextureBuffer(*this->context, shadowDepthTexture));

		TextureBufferSetting multisampleColorTexture = {
			.imageFormat = this->context->window->swapchainFormat,
			.imageExtent = this->context->window->swapchainExtent,
			.imageUsage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.viewAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
			.samples = VK_SAMPLE_COUNT_4_BIT };
		this->buffers.emplace("multisampleColour", createTextureBuffer(*this->context, multisampleColorTexture));

		TextureBufferSetting multisampleDepthTexture = {
			.imageFormat = VK_FORMAT_D32_SFLOAT_S8_UINT,
			.imageExtent = this->context->window->swapchainExtent,
			.imageUsage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT,
			.samples = VK_SAMPLE_COUNT_4_BIT };
		this->buffers.emplace("multisampleDepth", createTextureBuffer(*this->context, multisampleDepthTexture));

		// Framebuffers
		VkExtent2D swapchainExtent = this->context->window->swapchainExtent;
		VkExtent2D shadowExtent = VkExtent2D{ 2048, 2048 };

		std::vector<VkImageView> forwardViews;
		forwardViews.emplace_back(this->buffers["depth"].second.handle);
		createFramebuffers(*this->context->window, this->forwardFramebuffers, this->renderPasses["forward"].handle, forwardViews, swapchainExtent);

		std::vector<VkImageView> forwardMSAAViews;
		forwardMSAAViews.emplace_back(this->buffers["multisampleColour"].second.handle);
		forwardMSAAViews.emplace_back(this->buffers["multisampleDepth"].second.handle);
		createFramebuffers(*this->context->window, this->forwardMSAAFramebuffers, this->renderPasses["forwardMSAA"].handle, forwardMSAAViews, swapchainExtent);

		std::vector<VkImageView> deferredViews;
		// Must be emplaced in the vector in the *EXACT* same order as defined in the render pass
		deferredViews.emplace_back(this->buffers["normals"].second.handle);
		deferredViews.emplace_back(this->buffers["albedo"].second.handle);
		deferredViews.emplace_back(this->buffers["emissive"].second.handle);
		deferredViews.emplace_back(this->buffers["depth"].second.handle);
		createFramebuffers(*this->context->window, this->deferredFramebuffers, this->renderPasses["deferred"].handle, deferredViews, swapchainExtent);

		std::vector<VkImageView> shadowViews;
		shadowViews.emplace_back(this->buffers["shadowDepth"].second.handle);
		createFramebuffers(*this->context->window, this->shadowFramebuffer, this->renderPasses["shadow"].handle, shadowViews, shadowExtent, true);

		std::vector<VkImageView> swapchainViews;
		createFramebuffers(*this->context->window, this->crosshairFramebuffer, this->renderPasses["crosshair"].handle, swapchainViews, swapchainExtent);

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
		this->uniformBuffers.emplace("orthoMatrices",
			vk::createBuffer(
				"orthoMatrices",
				*this->context->allocator,
				sizeof(glsl::OrthoMatrices),
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
				this->descriptorLayouts["fragImageLayout"].handle,
				this->buffers["shadowDepth"].second.handle,
				this->depthSampler.handle));
		this->descriptorSets.emplace("orthoMatrices",
			createUBODescriptor(
				*this->context->window,
				this->descriptorLayouts["orthoMatrices"].handle,
				this->uniformBuffers["orthoMatrices"].buffer));

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

	void Renderer::initialiseJointMatrices() {
		std::vector<vk::Model>& models = this->game->GetModels();

		this->isSceneLoaded = true;

		std::vector<std::unique_ptr<ComponentBase>>* renderComponents = this->entityManager->GetComponentsOfType(RENDER);
		if (renderComponents == nullptr)
			return;

		for (std::size_t i = 0; i < renderComponents->size(); i++) {
			RenderComponent* renderComponent = reinterpret_cast<RenderComponent*>((*renderComponents)[i].get());
			int modelIndex = renderComponent->GetModelIndex();
			vk::Model& model = models[modelIndex];

			for (vk::Node* node : model.linearNodes) {
				if (!node->mesh)
					continue;

				node->descriptorBuffer = vk::createBuffer(
					"jointMatrix",
					*this->context->allocator,
					sizeof(glsl::SkinningUniform),
					VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					0,
					VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
				node->descriptor = createUBODescriptor(*this->context->window, this->descriptorLayouts["vertUBOLayout"].handle, node->descriptorBuffer.buffer);
			}
		}
	}

	void Renderer::cleanModelMatrices() {
		this->isSceneLoaded = false;
	}

	void Renderer::attachCameraComponent(CameraComponent* cameraComponent) {
		this->cameraComponent = cameraComponent;
		this->camera = cameraComponent->GetCamera();
	}

	void Renderer::initialiseModelDescriptors(std::vector<vk::Model>& aModels) {
		for (vk::Model& model : aModels)
			model.createDescriptorSets(*this->context, this->descriptorLayouts["materialLayout"].handle, this->descriptorLayouts["SSBOLayout"].handle);
	}

	bool Renderer::checkSwapchain() {
		if (this->recreateSwapchain) {
			// Handle minimisation
			int width, height;
			glfwGetFramebufferSize(this->context->getGLFWWindow(), &width, &height);
			// Loop indefinitely until framebuffer size becomes non-zero (window is no longer minimised)
			while (width == 0 || height == 0) {
				glfwGetFramebufferSize(this->context->getGLFWWindow(), &width, &height);
				glfwWaitEvents();
			}

			vkDeviceWaitIdle(this->context->window->device->device);

			// Recreate swapchain stuff
			const auto changes = Engine::recreateSwapchain(*this->context->window, this->vsync ? VK_PRESENT_MODE_FIFO_KHR : VK_PRESENT_MODE_IMMEDIATE_KHR);

			// We also need to recreate format and size dependents if MSAA is changed
			if (changes.changedFormat || this->game->GetGUI().changedMSAA)
				this->recreateFormatDependents();

			if (changes.changedSize || this->game->GetGUI().changedMSAA)
				this->recreateSizeDependents();

			// If we changed MSAA setting, recreate ImGui context to match MSAA samples
			if (this->game->GetGUI().changedMSAA) {
				this->destroyImGui();
				this->game->GetGUI().initGUI();
				this->game->GetGUI().changedMSAA = false;
			}

			this->recreateOthers();

			// Update crosshair positons
			((FPSTest*)this->game)->getCrosshair().shouldUpdateCrosshair = true;

			// Destroy and recreate all semaphores so none are in signaled state, which
			// may be the case when vkAcquireNextImageKHR returns VK_SUBOPTIMAL_KHR.
			this->imageAvailable.clear();
			for (std::size_t i = 0; i < this->context->window->swapViews.size(); i++)
				this->imageAvailable.emplace_back(createSemaphore(*this->context->window));

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

	void Renderer::updateAnimations(float timeDelta) {
		std::vector<vk::Model>& models = this->game->GetModels();

		std::vector<std::unique_ptr<ComponentBase>>* renderComponents = this->entityManager->GetComponentsOfType(RENDER);
		if (renderComponents == nullptr)
			return;

		for (std::size_t i = 0; i < renderComponents->size(); i++) {
			RenderComponent* renderComponent = reinterpret_cast<RenderComponent*>((*renderComponents)[i].get());
			int modelIndex = renderComponent->GetModelIndex();
			vk::Model& model = models[modelIndex];

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
		glm::mat4 depthProjection = glm::perspective(glm::radians(45.0f), 1.0f, 1.0f, 96.0f);
		glm::mat4 depthView = glm::lookAt(glm::vec3(0.75f, 20.0f, -0.4f), glm::vec3(0.75f, 0.0f, -0.4f), glm::vec3(1.0f, 0.0f, 0.0f));
		//glm::mat4 depthModel = glm::mat4(1.0f); // Model matrix is just identity

		this->uniforms.depthMVP.depthMVP = depthProjection * depthView;

		this->uniforms.orthoMatrices.projection = glm::ortho(0.0f, width, height, 0.0f);

		if (isSceneLoaded)
			updateModelMatrices();
	}

	void Renderer::updateModelMatrices() {
		std::vector<vk::Model>& models = this->game->GetModels();

		std::vector<int> renderEntities = this->entityManager->GetEntitiesWithComponent(RENDER);
		for (std::size_t i = 0; i < renderEntities.size(); i++) {
			RenderComponent* renderComponent = reinterpret_cast<RenderComponent*>(this->entityManager->GetComponentOfEntity(renderEntities[i], RENDER));
			if (!renderComponent->GetIsActive())
				continue;
			int modelIndex = renderComponent->GetModelIndex();
			vk::Model& model = models[modelIndex];

			for (std::size_t j = 0; j < model.linearNodes.size(); j++) {
				// Get the entities model matrix (post transform) and times it by the models model matrix (the one from the glTF file) 
				model.linearNodes[j]->globalMatrix = this->entityManager->GetEntity(renderEntities[i])->GetModelMatrix() * model.linearNodes[j]->getModelMatrix();
			}
		}
	}

	void Renderer::render(std::vector<vk::Model>& models) {
		unsigned int modes = *this->game->GetRenderModes();
		if ((modes & (1 << GUIHOME)) || (modes & (1 << GUISETTINGS)) || (modes & (1 << GUILOADING)) || ((modes & (1 << GUISERVER)) && (modes & (1 << GUIDEBUG)))) {
			this->renderGUI();
		}
		else if (modes & (1 << FORWARD)) {
			this->renderForward(modes & (1 << GUIDEBUG));
		}
		else if (modes & (1 << DEFERRED)) {
			this->renderDeferred(modes & (1 << GUIDEBUG));
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

		this->destroyImGui();
	}

	void Renderer::destroyImGui() {
		ImGui_ImplVulkan_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void Renderer::renderGUI() {
		VkCommandBuffer cmdBuf = this->cmdBuffers[this->frameIndex];

		// Begin recording
		beginCommandBuffer(cmdBuf, VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT);

		// Clear attachments
		std::vector<VkClearValue> clearValues;
		VkClearValue colourClearValue{};
		colourClearValue.color = { {0.1f, 0.1f, 0.1f, 1.0f} };
		VkClearValue depthClearValue{};
		depthClearValue.depthStencil.depth = 1.0f;

		bool msaaFlag = this->msaaIndex != 0;

		if (msaaFlag) {
			clearValues.emplace_back(colourClearValue);
			clearValues.emplace_back(colourClearValue);
			clearValues.emplace_back(depthClearValue);
		}
		else {
			clearValues.emplace_back(colourClearValue);
			clearValues.emplace_back(depthClearValue);
		}

		std::string renderPass = "forward";
		if (msaaFlag)
			renderPass += "MSAA";

		// Initialise render pass
		VkRenderPassBeginInfo passInfo{};
		passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passInfo.renderPass = this->renderPasses[renderPass].handle;
		passInfo.framebuffer = msaaFlag ? this->forwardMSAAFramebuffers[this->imageIndex].handle : this->forwardFramebuffers[this->imageIndex].handle;
		passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo.renderArea.extent = this->context->window->swapchainExtent;
		passInfo.clearValueCount = (uint32_t)clearValues.size();
		passInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);

		vkCmdEndRenderPass(cmdBuf);

		if (const auto res = vkEndCommandBuffer(cmdBuf); VK_SUCCESS != res)
			throw Utils::Error("Unable to end command buffer\n vkEndCommandBuffer() returned %s", Utils::toString(res).c_str());
	}

	void Renderer::renderForward(bool debug) {
		std::vector<vk::Model>& models = this->game->GetModels();
		unsigned int modes = *this->game->GetRenderModes();
		bool shadow = modes & (1 << SHADOWS);

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

		std::vector<std::unique_ptr<ComponentBase>>* renderComponents = this->entityManager->GetComponentsOfType(RENDER);
		if (renderComponents == nullptr)
			return;

		for (std::size_t i = 0; i < renderComponents->size(); i++) {
			RenderComponent* renderComponent = reinterpret_cast<RenderComponent*>((*renderComponents)[i].get());

			int modelIndex = renderComponent->GetModelIndex();
			vk::Model& model = models[modelIndex];

			for (vk::Node* node : model.linearNodes) {
				if (!node->mesh)
					continue;

				Utils::bufferBarrier(
					cmdBuf,
					node->descriptorBuffer.buffer,
					VK_ACCESS_UNIFORM_READ_BIT,
					VK_ACCESS_TRANSFER_WRITE_BIT,
					VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT
				);

				vkCmdUpdateBuffer(cmdBuf, node->descriptorBuffer.buffer, 0, sizeof(glsl::SkinningUniform), &node->skinUniform);

				Utils::bufferBarrier(
					cmdBuf,
					node->descriptorBuffer.buffer,
					VK_ACCESS_TRANSFER_WRITE_BIT,
					VK_ACCESS_UNIFORM_READ_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
				);
			}
		}

		if (shadow) {
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
			passInfoS.framebuffer = this->shadowFramebuffer[0].handle;
			passInfoS.renderArea.offset = VkOffset2D{ 0, 0 };
			passInfoS.renderArea.extent = VkExtent2D{ 2048, 2048 };
			passInfoS.clearValueCount = 1;
			passInfoS.pClearValues = clearValueS;

			vkCmdBeginRenderPass(cmdBuf, &passInfoS, VK_SUBPASS_CONTENTS_INLINE);

			vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["shadow"].handle);

			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["shadow"].handle, 0, 1, &this->descriptorSets["shadow"], 0, nullptr); // Depth matrix

			vkCmdSetDepthBias(cmdBuf, this->depthBiasConstant, 0.0f, this->depthBiasSlopeFactor);

			drawModels(cmdBuf, "shadow", true);

			vkCmdEndRenderPass(cmdBuf);
		}

		// Clear attachments
		std::vector<VkClearValue> clearValues;
		VkClearValue colourClearValue{};
		colourClearValue.color = { {0.1f, 0.1f, 0.1f, 1.0f} };
		VkClearValue depthClearValue{};
		depthClearValue.depthStencil.depth = 1.0f;

		bool msaaFlag = this->msaaIndex != 0;

		if (msaaFlag) {
			clearValues.emplace_back(colourClearValue);
			clearValues.emplace_back(colourClearValue);
			clearValues.emplace_back(depthClearValue);
		}
		else {
			clearValues.emplace_back(colourClearValue);
			clearValues.emplace_back(depthClearValue);
		}

		std::string renderPass = "forward";
		std::string pipeline = "forward";
		std::string pipelineLayout = "forward";
		if (shadow) {
			pipeline += "Shadow";
			pipelineLayout += "Shadow";
		}
		if (msaaFlag) {
			renderPass += "MSAA";
			pipeline += "MSAA";
		}

		// Initialise render pass
		VkRenderPassBeginInfo passInfo{};
		passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passInfo.renderPass = this->renderPasses[renderPass].handle;
		passInfo.framebuffer = msaaFlag ? this->forwardMSAAFramebuffers[this->imageIndex].handle : this->forwardFramebuffers[this->imageIndex].handle;
		passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo.renderArea.extent = this->context->window->swapchainExtent;
		passInfo.clearValueCount = (uint32_t)clearValues.size();
		passInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

		if (this->skybox) {
			std::string skyboxPipeline = msaaFlag ? "skyboxMSAA" : "skybox";

			vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines[skyboxPipeline].handle);

			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["skybox"].handle, 0, 1, &this->descriptorSets["scene"], 0, nullptr);
			this->skybox->bind(cmdBuf, this->pipelineLayouts["skybox"].handle);

			vkCmdDraw(cmdBuf, 36, 1, 0, 0);
		}

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines[pipeline].handle);

		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts[pipelineLayout].handle, 0, 1, &this->descriptorSets["scene"], 0, nullptr); // Projective matrices

		if (shadow) {
			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts[pipelineLayout].handle, 4, 1, &this->descriptorSets["shadow"], 0, nullptr); // Depth matrix
			vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts[pipelineLayout].handle, 5, 1, &this->descriptorSets["shadowMap"], 0, nullptr); // Shadow map
		}

		drawModels(cmdBuf, pipelineLayout);

		if (debug) {
			ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);
		}

		vkCmdEndRenderPass(cmdBuf);

		// Draw crosshair
		((FPSTest*)this->game)->getCrosshair().updatePositions();

		Utils::bufferBarrier(
			cmdBuf,
			this->uniformBuffers["orthoMatrices"].buffer,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT
		);

		vkCmdUpdateBuffer(cmdBuf, this->uniformBuffers["orthoMatrices"].buffer, 0, sizeof(glsl::OrthoMatrices), &this->uniforms.orthoMatrices);

		Utils::bufferBarrier(
			cmdBuf,
			this->uniformBuffers["orthoMatrices"].buffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
		);

		clearValues.clear();
		clearValues.emplace_back(colourClearValue);

		VkRenderPassBeginInfo passInfo2{};
		passInfo2.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passInfo2.renderPass = this->renderPasses["crosshair"].handle;
		passInfo2.framebuffer = this->crosshairFramebuffer[this->imageIndex].handle;
		passInfo2.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo2.renderArea.extent = this->context->window->swapchainExtent;
		passInfo2.clearValueCount = (uint32_t)clearValues.size();
		passInfo2.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(cmdBuf, &passInfo2, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["crosshair"].handle);

		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["crosshair"].handle, 0, 1, &this->descriptorSets["orthoMatrices"], 0, nullptr);

		((FPSTest*)this->game)->getCrosshair().drawCrosshair(cmdBuf);

		vkCmdEndRenderPass(cmdBuf);

		if (const auto res = vkEndCommandBuffer(cmdBuf); VK_SUCCESS != res)
			throw Utils::Error("Unable to end command buffer\n vkEndCommandBuffer() returned %s", Utils::toString(res).c_str());
	}

	void Renderer::renderDeferred(bool debug) {
		std::vector<vk::Model>& models = this->game->GetModels();

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

		std::vector<std::unique_ptr<ComponentBase>>* renderComponents = this->entityManager->GetComponentsOfType(RENDER);
		if (renderComponents == nullptr)
			return;

		for (std::size_t i = 0; i < renderComponents->size(); i++) {
			RenderComponent* renderComponent = reinterpret_cast<RenderComponent*>((*renderComponents)[i].get());
			int modelIndex = renderComponent->GetModelIndex();
			vk::Model& model = models[modelIndex];

			for (vk::Node* node : model.linearNodes) {
				if (!node->mesh)
					continue;

				Utils::bufferBarrier(
					cmdBuf,
					node->descriptorBuffer.buffer,
					VK_ACCESS_UNIFORM_READ_BIT,
					VK_ACCESS_TRANSFER_WRITE_BIT,
					VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT
				);

				vkCmdUpdateBuffer(cmdBuf, node->descriptorBuffer.buffer, 0, sizeof(glsl::SkinningUniform), &node->skinUniform);

				Utils::bufferBarrier(
					cmdBuf,
					node->descriptorBuffer.buffer,
					VK_ACCESS_TRANSFER_WRITE_BIT,
					VK_ACCESS_UNIFORM_READ_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_VERTEX_SHADER_BIT
				);
			}
		}

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
		passInfo.renderArea.extent = this->context->window->swapchainExtent;
		passInfo.clearValueCount = 5;
		passInfo.pClearValues = clearValues;

		vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["gBufWrite"].handle);

		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["forward"].handle, 0, 1, &this->descriptorSets["scene"], 0, nullptr);

		drawModels(cmdBuf, "forward"); // The gBufWrite stage uses same pipelineLayout as forward

		vkCmdNextSubpass(cmdBuf, VK_SUBPASS_CONTENTS_INLINE);

		vkCmdBindPipeline(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelines["deferred"].handle);
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["deferred"].handle, 0, 1, &this->descriptorSets["deferredShading"], 0, nullptr);
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["deferred"].handle, 1, 1, &this->descriptorSets["scene"], 0, nullptr);
		vkCmdBindDescriptorSets(cmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, this->pipelineLayouts["deferred"].handle, 2, 1, &this->descriptorSets["lights"], 0, nullptr);

		vkCmdDraw(cmdBuf, 3, 1, 0, 0);

		//if (debug)
			//ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmdBuf);

		vkCmdEndRenderPass(cmdBuf);

		if (const auto res = vkEndCommandBuffer(cmdBuf); VK_SUCCESS != res)
			throw Utils::Error("Unable to end command buffer\n vkEndCommandBuffer() returned %s", Utils::toString(res).c_str());
	}

	void Renderer::recreateFormatDependents() {
		VkSampleCountFlagBits sampleCount = this->context->window->device->getSampleCount(this->msaaIndex);

		this->renderPasses["forward"] = createRenderPass(*this->context->window);
		this->renderPasses["forwardMSAA"] = createRenderPassMSAA(*this->context->window, Utils::getMSAAMinimum(sampleCount));
		this->renderPasses["deferred"] = createDeferredRenderPass(*this->context->window);
		this->renderPasses["shadow"] = createShadowRenderPass(*this->context->window);
		this->renderPasses["crosshair"] = createCrosshairRenderPass(*this->context->window);
	}

	void Renderer::recreateSizeDependents() {
		VkSampleCountFlagBits sampleCount = this->context->window->device->getSampleCount(this->msaaIndex);

		TextureBufferSetting depthTexture = {
			.imageFormat = VK_FORMAT_D32_SFLOAT_S8_UINT,
			.imageExtent = this->context->window->swapchainExtent,
			.imageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT };
		this->buffers["depth"] = createTextureBuffer(*this->context, depthTexture);

		TextureBufferSetting colorTexture = {
			.imageFormat = VK_FORMAT_R16G16B16A16_SFLOAT,
			.imageExtent = this->context->window->swapchainExtent,
			.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
			.viewAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT };
		this->buffers["normals"] = createTextureBuffer(*this->context, colorTexture);
		this->buffers["albedo"] = createTextureBuffer(*this->context, colorTexture);
		this->buffers["emissive"] = createTextureBuffer(*this->context, colorTexture);

		TextureBufferSetting shadowDepthTexture = {
			.imageFormat = VK_FORMAT_D32_SFLOAT_S8_UINT,
			.imageExtent = VkExtent2D {2048, 2048},
			.imageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT };
		this->buffers["shadowDepth"] = createTextureBuffer(*this->context, shadowDepthTexture);

		TextureBufferSetting multisampleColorTexture = {
			.imageFormat = this->context->window->swapchainFormat,
			.imageExtent = this->context->window->swapchainExtent,
			.imageUsage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
			.viewAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
			.samples = Utils::getMSAAMinimum(sampleCount) };
		this->buffers["multisampleColour"] = createTextureBuffer(*this->context, multisampleColorTexture);

		TextureBufferSetting multisampleDepthTexture = {
			.imageFormat = VK_FORMAT_D32_SFLOAT_S8_UINT,
			.imageExtent = this->context->window->swapchainExtent,
			.imageUsage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
			.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT,
			.samples = Utils::getMSAAMinimum(sampleCount) };
		this->buffers["multisampleDepth"] = createTextureBuffer(*this->context, multisampleDepthTexture);

		std::tuple<vk::Pipeline, vk::Pipeline> deferredPipelines = createDeferredPipelines(
			*this->context->window,
			this->renderPasses["deferred"].handle,
			this->pipelineLayouts["forward"].handle,
			this->pipelineLayouts["deferred"].handle);

		this->pipelines["forward"] = createForwardPipeline(*this->context->window, this->renderPasses["forward"].handle, this->pipelineLayouts["forward"].handle, false);
		this->pipelines["forwardMSAA"] = createForwardPipeline(*this->context->window, this->renderPasses["forwardMSAA"].handle, this->pipelineLayouts["forward"].handle, false, Utils::getMSAAMinimum(sampleCount));
		this->pipelines["forwardShadow"] = createForwardPipeline(*this->context->window, this->renderPasses["forward"].handle, this->pipelineLayouts["forwardShadow"].handle, true);
		this->pipelines["forwardShadowMSAA"] = createForwardPipeline(*this->context->window, this->renderPasses["forwardMSAA"].handle, this->pipelineLayouts["forwardShadow"].handle, true, Utils::getMSAAMinimum(sampleCount));
		this->pipelines["gBufWrite"] = std::move(std::get<0>(deferredPipelines));
		this->pipelines["deferred"] = std::move(std::get<1>(deferredPipelines));
		this->pipelines["shadow"] = createShadowOffscreenPipeline(*this->context->window, this->renderPasses["shadow"].handle, this->pipelineLayouts["shadow"].handle);
		this->pipelines["skybox"] = createSkyboxPipeline(*this->context->window, this->renderPasses["forward"].handle, this->pipelineLayouts["skybox"].handle);
		this->pipelines["skyboxMSAA"] = createSkyboxPipeline(*this->context->window, this->renderPasses["forwardMSAA"].handle, this->pipelineLayouts["skybox"].handle, Utils::getMSAAMinimum(sampleCount));
		this->pipelines["crosshair"] = createCrosshairPipeline(*this->context->window, this->renderPasses["crosshair"].handle, this->pipelineLayouts["crosshair"].handle);
	}

	void Renderer::recreateOthers() {
		VkExtent2D swapchainExtent = this->context->window->swapchainExtent;
		VkExtent2D shadowExtent = VkExtent2D{ 2048, 2048 };

		this->forwardFramebuffers.clear();
		this->forwardMSAAFramebuffers.clear();
		this->deferredFramebuffers.clear();
		this->shadowFramebuffer.clear();
		this->crosshairFramebuffer.clear();

		std::vector<VkImageView> forwardViews;
		forwardViews.emplace_back(this->buffers["depth"].second.handle);
		createFramebuffers(*this->context->window, this->forwardFramebuffers, this->renderPasses["forward"].handle, forwardViews, swapchainExtent);

		std::vector<VkImageView> forwardMSAAViews;
		forwardMSAAViews.emplace_back(this->buffers["multisampleColour"].second.handle);
		forwardMSAAViews.emplace_back(this->buffers["multisampleDepth"].second.handle);
		createFramebuffers(*this->context->window, this->forwardMSAAFramebuffers, this->renderPasses["forwardMSAA"].handle, forwardMSAAViews, swapchainExtent);

		std::vector<VkImageView> deferredViews;
		// Must be emplaced in the vector in the *EXACT* same order as defined in the render pass
		deferredViews.emplace_back(this->buffers["normals"].second.handle);
		deferredViews.emplace_back(this->buffers["albedo"].second.handle);
		deferredViews.emplace_back(this->buffers["emissive"].second.handle);
		deferredViews.emplace_back(this->buffers["depth"].second.handle);
		createFramebuffers(*this->context->window, this->deferredFramebuffers, this->renderPasses["deferred"].handle, deferredViews, swapchainExtent);

		std::vector<VkImageView> shadowViews;
		shadowViews.emplace_back(this->buffers["shadowDepth"].second.handle);
		createFramebuffers(*this->context->window, this->shadowFramebuffer, this->renderPasses["shadow"].handle, shadowViews, shadowExtent, true);

		std::vector<VkImageView> swapchainViews;
		createFramebuffers(*this->context->window, this->crosshairFramebuffer, this->renderPasses["crosshair"].handle, swapchainViews, swapchainExtent);

		this->descriptorSets["deferredShading"] = createDeferredShadingDescriptor(
			*this->context->window,
			this->descriptorLayouts["deferredLayout"].handle,
			this->buffers["depth"].second.handle,
			this->buffers["normals"].second.handle,
			this->buffers["albedo"].second.handle,
			this->buffers["emissive"].second.handle);
		this->descriptorSets["shadowMap"] = createImageDescriptor(
			*this->context->window,
			this->descriptorLayouts["fragImageLayout"].handle,
			this->buffers["shadowDepth"].second.handle,
			this->depthSampler.handle);
	}

	void Renderer::drawModels(VkCommandBuffer cmdBuf, std::string handle, bool justGeometry) {
		std::vector<vk::Model>& models = this->game->GetModels();

		std::vector<std::unique_ptr<ComponentBase>>* renderComponents = this->entityManager->GetComponentsOfType(RENDER);
		if (renderComponents == nullptr)
			return;

		for (std::size_t i = 0; i < renderComponents->size(); i++) {
			RenderComponent* renderComponent = reinterpret_cast<RenderComponent*>((*renderComponents)[i].get());
			if (!renderComponent->GetIsActive())
				continue;
			int modelIndex = renderComponent->GetModelIndex();
			models[modelIndex].drawModel(cmdBuf, this, handle, justGeometry);
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
			std::string title = std::format("FPS Test Game - Avg FPS: {} - Avg Frame Time: {:.3f}ms", this->avgFps, this->avgFrameTime * 1000.0f);
			glfwSetWindowTitle(this->context->getGLFWWindow(), title.c_str());
		}
		this->prevTime = currTime;
	}

	void Renderer::setRecreateSwapchain(bool value) {
		this->recreateSwapchain = value;
	}

	std::map<std::string, vk::PipelineLayout>& Renderer::getPipelineLayouts() {
		return this->pipelineLayouts;
	}

	vk::PipelineLayout& Renderer::getPipelineLayout(const std::string& handle) {
		return this->pipelineLayouts[handle];
	}

	std::map<std::string, vk::Pipeline>& Renderer::getPipelines() {
		return this->pipelines;
	}

	vk::Pipeline& Renderer::getPipeline(const std::string& handle) {
		return this->pipelines[handle];
	}

	std::map<std::string, VkDescriptorSet> Renderer::getDescriptorSets() {
		return this->descriptorSets;
	}

	VkDescriptorSet Renderer::getDescriptorSet(const std::string& handle) {
		return this->descriptorSets[handle];
	}

	float Renderer::getAvgFrameTime() {
		return this->avgFrameTime;
	}

	int Renderer::getAvgFPS() {
		return this->avgFps;
	}

	std::pair<const char**, int> Renderer::getMSAAOptions() {
		return { this->msaaOptions.data(), (int)this->msaaOptions.size() };
	}
}
#pragma once

#include "../Core/RenderMode.hpp"
#include "objects/Model.hpp"
#include "Uniforms.hpp"
#include "Utils.hpp"
#include "../ECS/EntityManager.hpp"
#include "../Core/Camera.hpp"
#include "../Core/Game.hpp"

namespace Engine
{
	class Game;
}

namespace Engine {

	class Camera;

	struct Uniforms {
		glsl::SceneUniform sceneUniform;
		glsl::ModelMatricesUniform modelMatricesUniform;
		glsl::MaterialInfoBuffer materialInfoBuffer;
		glsl::LightsUniform lightsUniform;
		glsl::DepthMVP depthMVP;
	};

	class Renderer {
	public:
		Renderer(VulkanContext* aContext, EntityManager* entityManager, Engine::Game* game);
		Renderer() = default;

		VkRenderPass& GetRenderPass(std::string s);
		Engine::Camera* GetCamera() { return camera; }
		bool const GetIsSceneLoaded() { return isSceneLoaded; }

		void initialiseRenderer();
		void initialiseModelMatrices();
		void cleanModelMatrices();
		void attachCamera(Engine::Camera* camera);
		void initialiseModelDescriptors(std::vector<vk::Model>& models);
		bool checkSwapchain();
		bool acquireSwapchainImage();
		void updateUniforms();
		void updateModelMatrices();
		void render(std::vector<vk::Model>& models);
		void submitRender();
		void finishRendering();

		vk::Buffer createDynamicUniformBuffer();
		void modeOn(Engine::RenderMode r);
		void modeOff(Engine::RenderMode r);

	private:
		VulkanContext* context;
		EntityManager* entityManager;
		Engine::Game* game;
		Engine::Camera* camera;

		std::map<std::string, vk::RenderPass> renderPasses;
		std::map<std::string, vk::DescriptorSetLayout> descriptorLayouts;
		std::map<std::string, vk::PipelineLayout> pipelineLayouts;
		std::map<std::string, vk::Pipeline> pipelines;
		std::map<std::string, std::pair<vk::Texture, vk::ImageView>> buffers;
		std::map<std::string, vk::Buffer> uniformBuffers;
		std::map<std::string, VkDescriptorSet> descriptorSets;

		vk::Sampler depthSampler;

		std::vector<vk::Framebuffer> defaultFramebuffers;
		std::vector<vk::Framebuffer> deferredFramebuffers;
		std::vector<vk::Framebuffer> shadowFramebuffer;

		std::unordered_map<Engine::RenderMode, std::vector<vk::Framebuffer>*> framebuffersMap = {
			{Engine::RenderMode::FORWARD, &defaultFramebuffers},
			{Engine::RenderMode::DEFERRED,& deferredFramebuffers},
			{Engine::RenderMode::SHADOWS, &shadowFramebuffer}
		};

		std::size_t frameIndex = 0;
		std::uint32_t imageIndex = 0;
		std::vector<VkCommandBuffer> cmdBuffers;
		std::vector<vk::Fence> frameDone;
		std::vector<vk::Semaphore> imageAvailable;
		std::vector<vk::Semaphore> renderFinished;

		std::size_t dynamicUBOAlignment;

		Uniforms uniforms;

		bool isSceneLoaded = false;

		bool recreateSwapchain = false;

		void renderGUI();
		void renderForward(std::vector<vk::Model>& models, bool debug);
		void renderForwardShadows(std::vector<vk::Model>& models, bool debug);
		void renderDeferred(std::vector<vk::Model>& models, bool debug);
		void drawModels(VkCommandBuffer& cmdBuf, std::vector<vk::Model>& models, std::string handle);

		void recreateFormatDependents();
		void recreateSizeDependents();
		void recreateOthers();
	};
}
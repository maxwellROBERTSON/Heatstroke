#pragma once

#include "Uniforms.hpp"
#include "Utils.hpp"
#include "../gltf/Model.hpp"
#include "../ECS/EntityManager.hpp"
#include "../Core/RenderMode.hpp"
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
		Renderer(VulkanContext* aContext, EntityManager* entityManager, Game* game);
		Renderer() = default;

		VkRenderPass& GetRenderPass(std::string s);
		Camera* GetCameraPointer() { return camera; }
		bool const GetIsSceneLoaded() { return isSceneLoaded; }

		void initialiseRenderer();
		void initialiseModelMatrices();
		void initialiseJointMatrices();
		void cleanModelMatrices();
		void attachCamera(Camera* camera);
		void initialiseModelDescriptors(std::vector<vk::Model>& models);
		bool checkSwapchain();
		bool acquireSwapchainImage();
		void updateAnimations(float timeDelta);
		void updateUniforms();
		void updateModelMatrices();
		void render(std::vector<vk::Model>& models);
		void submitRender();
		void finishRendering();
		void destroyImGui();

		vk::Buffer createDynamicUniformBuffer();
		void calculateFPS();

		// Setters
		void setRecreateSwapchain(bool value);

		// Getters
		std::map<std::string, vk::PipelineLayout>& getPipelineLayouts();
		vk::PipelineLayout& getPipelineLayout(const std::string& handle);

		std::map<std::string, vk::Pipeline>& getPipelines();
		vk::Pipeline& getPipeline(const std::string& handle);

		std::map<std::string, VkDescriptorSet> getDescriptorSets();
		VkDescriptorSet getDescriptorSet(const std::string& handle);

		std::size_t getDynamicUBOAlignment();

		float getAvgFrameTime();
		int getAvgFPS();

		// Debug things
		float depthBiasConstant = 7.0f;
		float depthBiasSlopeFactor = 10.0f;

		bool vsync = true;
		bool msaa = false;
		bool delayMSAA = false;

	private:
		VulkanContext* context;
		EntityManager* entityManager;
		Game* game;
		Camera* camera;

		std::map<std::string, vk::RenderPass> renderPasses;
		std::map<std::string, vk::DescriptorSetLayout> descriptorLayouts;
		std::map<std::string, vk::PipelineLayout> pipelineLayouts;
		std::map<std::string, vk::Pipeline> pipelines;
		std::map<std::string, std::pair<vk::Texture, vk::ImageView>> buffers;
		std::map<std::string, vk::Buffer> uniformBuffers;
		std::map<std::string, VkDescriptorSet> descriptorSets;

		vk::Sampler depthSampler;

		std::vector<vk::Framebuffer> forwardFramebuffers;
		std::vector<vk::Framebuffer> forwardMSAAFramebuffers;
		std::vector<vk::Framebuffer> deferredFramebuffers;
		std::vector<vk::Framebuffer> shadowFramebuffer;

		std::size_t frameIndex = 0;
		std::uint32_t imageIndex = 0;
		std::vector<VkCommandBuffer> cmdBuffers;
		std::vector<vk::Fence> frameDone;
		std::vector<vk::Semaphore> imageAvailable;
		std::vector<vk::Semaphore> renderFinished;

		std::size_t dynamicUBOAlignment;
		// Number of model matrices when creating dynamic uniform buffer object
		std::size_t modelMatrices;

		Uniforms uniforms;

		bool isSceneLoaded = false;
		bool recreateSwapchain = false;

		// Frame times / fps variables
		float frameTime = 0.0f, avgFrameTime = 0.0f, prevTime = 0.1f, lastSecondTime = 0.0f;
		int avgFps = 0, frames = 0;
		std::vector<float> frameTimes;

		void renderGUI();
		void renderForward(std::vector<vk::Model>& models, bool debug);
		void renderDeferred(std::vector<vk::Model>& models, bool debug);
		void drawModels(VkCommandBuffer cmdBuf, std::vector<vk::Model>& models, std::string handle, bool justGeometry = false);

		void recreateFormatDependents();
		void recreateSizeDependents();
		void recreateOthers();
	};
}
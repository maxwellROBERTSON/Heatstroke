#pragma once

#include "Uniforms.hpp"
#include "Utils.hpp"
#include "Skybox.hpp"
#include "../gltf/Model.hpp"
#include "../ECS/EntityManager.hpp"
#include "../Core/RenderMode.hpp"
#include "../Core/Camera.hpp"
#include "../Core/Game.hpp"
#include "../ECS/Components/CameraComponent.hpp"

namespace Engine
{
	class Game;
}

namespace Engine {

	class Camera;

	struct Uniforms {
		glsl::SceneUniform sceneUniform;
		glsl::MaterialInfoBuffer materialInfoBuffer;
		glsl::LightsUniform lightsUniform;
		glsl::DepthMVP depthMVP;
		glsl::OrthoMatrices orthoMatrices;
	};

	class Renderer {
	public:
		Renderer(VulkanContext* aContext, EntityManager* entityManager, Game* game);
		Renderer() = default;

		void initialiseRenderer();
		void initialiseJointMatrices();
		void attachCameraComponent(Engine::CameraComponent* cameraComponent);
		void initialiseModelDescriptors();
		bool checkSwapchain();
		bool acquireSwapchainImage();
		void updateAnimations(float timeDelta);
		void updateUniforms();
		void updateModelMatrices();
		void render();
		void submitRender();
		void unloadScene();
		void finishRendering();
		void destroyImGui();

		void calculateFPS();

		// Setters
		void setRecreateSwapchain(bool value);
		void addSkybox(std::unique_ptr<Skybox> skybox);

		// Getters
		VkRenderPass& GetRenderPass(std::string s);
		CameraComponent* GetCameraComponentPointer() { return cameraComponent; }
		Camera* GetCameraPointer() { return camera; }
		bool const GetIsSceneLoaded() { return isSceneLoaded; }

		std::map<std::string, vk::PipelineLayout>& getPipelineLayouts();
		vk::PipelineLayout& getPipelineLayout(const std::string& handle);

		std::map<std::string, vk::Pipeline>& getPipelines();
		vk::Pipeline& getPipeline(const std::string& handle);

		std::map<std::string, VkDescriptorSet> getDescriptorSets();
		VkDescriptorSet getDescriptorSet(const std::string& handle);

		VkDescriptorSetLayout getDescriptorLayout(const std::string& handle);

		float getAvgFrameTime();
		int getAvgFPS();

		// Returns const char pointer to msaa option strings and number of msaa options
		std::pair<const char**, int> getMSAAOptions();

		bool vsync = true;
		int msaaIndex = 0;

		// Debug things
		float depthBiasConstant = 7.0f;
		float depthBiasSlopeFactor = 10.0f;
	private:
		VulkanContext* context;
		EntityManager* entityManager;
		Game* game;
		CameraComponent* cameraComponent;
		Camera* camera;

		std::unique_ptr<Skybox> skybox;

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
		std::vector<vk::Framebuffer> crosshairFramebuffer;

		std::size_t frameIndex = 0;
		std::uint32_t imageIndex = 0;
		std::vector<VkCommandBuffer> cmdBuffers;
		std::vector<vk::Fence> frameDone;
		std::vector<vk::Semaphore> imageAvailable;
		std::vector<vk::Semaphore> renderFinished;

		Uniforms uniforms;

		bool isSceneLoaded = false;
		bool recreateSwapchain = false;

		// Frame times / fps variables
		float frameTime = 0.0f, avgFrameTime = 0.0f, prevTime = 0.1f, lastSecondTime = 0.0f;
		int avgFps = 0, frames = 0;
		std::vector<float> frameTimes;

		std::vector<const char*> msaaOptions;

		void renderGUI();
		void renderForward(bool debug);
		void renderDeferred(bool debug);
		void drawModels(VkCommandBuffer cmdBuf, VkPipelineLayout pipelineLayout, DrawType drawType, bool justGeometry = false);

		void recreateFormatDependents();
		void recreateSizeDependents();
		void recreateOthers();
	};
}
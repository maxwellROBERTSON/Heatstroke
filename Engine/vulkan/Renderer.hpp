#pragma once

#include "objects/Model.hpp"
#include "Uniforms.hpp"
#include "Utils.hpp"
#include "../ECS/EntityManager.hpp"
#include "../Core/Camera.hpp"
//#include "../GUI/GUI.hpp"

namespace Engine {

	struct Uniforms {
		glsl::SceneUniform sceneUniform;
		glsl::ModelMatricesUniform modelMatricesUniform;
		glsl::MaterialInfoBuffer materialInfoBuffer;
		glsl::LightsUniform lightsUniform;
	};

	enum RenderMode {
		GUIX,
		FORWARD,
		DEFERRED
	};

	class Renderer {
	public:
		Renderer(VulkanContext* aContext, EntityManager* entityManager);
		Renderer() = default;

		VkRenderPass& GetRenderPass(std::string s);
		Camera* GetCamera() { return camera; }
		bool GetIsSceneLoaded() { return isSceneLoaded; }

		void initialiseRenderer();
		void initialiseModelMatrices();
		void cleanModelMatrices();
		void attachCamera(Camera* camera);
		void initialiseModelDescriptors(std::vector<vk::Model>& models);
		bool checkSwapchain();
		bool acquireSwapchainImage();
		void updateUniforms();
		void updateModelMatrices();
		void render(RenderMode renderMode, std::vector<vk::Model>& models);
		void submitRender();
		void finishRendering();

		vk::Buffer createDynamicUniformBuffer();

	private:
		VulkanContext* context;
		EntityManager* entityManager;
		Camera* camera;

		std::map<std::string, vk::RenderPass> renderPasses;
		std::map<std::string, vk::DescriptorSetLayout> descriptorLayouts;
		std::map<std::string, vk::PipelineLayout> pipelineLayouts;
		std::map<std::string, vk::Pipeline> pipelines;
		std::map<std::string, std::pair<vk::Texture, vk::ImageView>> buffers;
		std::map<std::string, vk::Buffer> uniformBuffers;
		std::map<std::string, VkDescriptorSet> descriptorSets;

		std::vector<vk::Framebuffer> defaultFramebuffers;
		std::vector<vk::Framebuffer> deferredFramebuffers;

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
		void renderForward(std::vector<vk::Model>& models);
		void renderDeferred(std::vector<vk::Model>& models);

		void recreateFormatDependents();
		void recreateSizeDependents();
		void recreateOthers();
	};
}
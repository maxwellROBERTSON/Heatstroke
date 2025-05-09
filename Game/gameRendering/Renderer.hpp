#pragma once

#include <stdint.h>
#include <map>
#include <string>

#include "../../Engine/Rendering/HsRenderPass.hpp"
#include "../../Engine/Rendering/HsPipeline.hpp"
#include "../../Engine/Rendering/HsPipelineLayout.hpp"
#include "../../Engine/Rendering/HsTextureBuffer.hpp"
#include "../../Engine/Rendering/HsUniformBuffer.hpp"
#include "../../Engine/Rendering/HsFramebuffer.hpp"
#include "../../Engine/vulkan/objects/VkObjects.hpp"

#include "../../Engine/vulkan/Skybox.hpp"

#include "../../Engine/gltf/Model.hpp"

namespace Engine {
	struct VulkanContext;
	class EntityManager;
	class Camera;
	class HsRenderer;
}

struct Uniforms {
	glsl::SceneUniform sceneUniform;
	glsl::DepthMVP depthMVP;
	glsl::OrthoMatrices orthoMatrices;
};

class FPSTest;

using RenderPass = std::unique_ptr<Engine::HsRenderPass>;
using PipelineLayout = std::unique_ptr<Engine::HsPipelineLayout>;
using Pipeline = std::unique_ptr<Engine::HsPipeline>;
using TextureBuffer = std::unique_ptr<Engine::HsTextureBuffer>;
using UniformBuffer = std::unique_ptr<Engine::HsUniformBuffer>;
using Framebuffer = std::unique_ptr<Engine::HsFramebuffer>;

class Renderer {
public:
	Renderer() = default;
	Renderer(Engine::VulkanContext* context, Engine::EntityManager* entityManager, FPSTest* game);

	// Initialisation
	void initialise();
	void attachCamera(Engine::Camera* camera);
	void initialiseJointMatrices();
	void initialiseModelDescriptors();

	// Swapchain
	bool checkSwapchain();
	bool acquireSwapchainImage();

	void updateAnimations(float timeDelta);
	void updateUniforms();
	void updateModelMatrices();
	void render();

	void submitRender();
	void finishRendering();

	void unloadScene();
	void destroyImGui();

	// Misc
	void calculateFPS();

	// Setters
	void setRecreateSwapchain(bool value);
	void addSkybox(std::unique_ptr<Engine::Skybox> skybox);

	// Getters
	VkRenderPass getRenderPassHandle(const std::string& renderPass);
	Engine::Camera* getCameraPointer();
	bool getIsSceneLoaded();
	int& getMSAAIndex();
	float getAvgFrameTime();
	int getAvgFPS();
	bool& getShadowState();

	VkDescriptorSetLayout getDescriptorLayout(const std::string& handle);

	// Returns const char pointer to msaa option strings and number of msaa options
	std::pair<const char**, int> getMSAAOptions();

	bool vsync = true;

	// Debug things
	float depthBiasConstant = 7.0f;
	float depthBiasSlopeFactor = 10.0f;
private:
	Engine::VulkanContext* context;
	Engine::EntityManager* entityManager;
	FPSTest* game;
	Engine::Camera* camera;

	std::unique_ptr<Engine::Skybox> skybox;

	std::map<std::string, RenderPass> renderPasses;
	std::map<std::string, Engine::vk::DescriptorSetLayout> descriptorLayouts;
	std::map<std::string, PipelineLayout> pipelineLayouts;
	std::map<std::string, Pipeline> pipelines;
	std::map<std::string, TextureBuffer> textureBuffers;
	std::map<std::string, UniformBuffer> uniformBuffers;
	std::map<std::string, VkDescriptorSet> descriptorSets;
	std::map<std::string, Framebuffer> framebuffers;

	Engine::vk::Sampler depthSampler;

	// Synchronisation variables
	std::uint32_t frameIndex = 0;
	std::uint32_t imageIndex = 0;
	std::vector<VkCommandBuffer> cmdBuffers;
	std::vector<Engine::vk::Fence> frameDoneFences;
	std::vector<Engine::vk::Semaphore> imageAvailableSemaphores;
	std::vector<Engine::vk::Semaphore> renderFinishedSemaphores;

	Uniforms uniforms;

	bool isSceneLoaded = false;
	bool recreateSwapchain = false;

	// Frame times / fps variables
	float frameTime = 0.0f, avgFrameTime = 0.0f, prevTime = 0.1f, lastSecondTime = 0.0f;
	int avgFps = 0, frames = 0;
	std::vector<float> frameTimes;

	std::vector<const char*> msaaOptions;

	// Renderer settings
	VkSampleCountFlagBits sampleCountSetting = VK_SAMPLE_COUNT_1_BIT;
	bool shadowsEnabled = false;
	VkExtent2D shadowResolution = VkExtent2D{ 2048, 2048 };
	int msaaIndex = 0;

	void renderGUI();
	void renderForward();
	void drawModels(VkCommandBuffer cmdBuf, VkPipelineLayout pipelineLayout, Engine::DrawType drawType, bool justGeometry = false);

	void recreateFormatDependents();
	void recreateSizeDependents();
	void recreateOthers();
};
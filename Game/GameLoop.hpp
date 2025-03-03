#pragma once

#include "../Engine/vulkan/VulkanContext.hpp"
#include "../Engine/vulkan/VulkanAllocator.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"
#include "../Engine/glfw/Callbacks.hpp"
#include "../Engine/vulkan/VulkanWindow.hpp"
#include "../Engine/vulkan/Renderer.hpp"
#include "../Engine/vulkan/PipelineCreation.hpp"
#include "../Engine/vulkan/objects/Buffer.hpp"
#include "../Engine/gltf/glTF.hpp"

#include "Camera.hpp"

void initialiseGame();
void initialiseModels(std::vector<Engine::vk::Model>& models);
void runGameLoop(std::vector<Engine::vk::Model>& models);

void updateSceneUniform(glsl::SceneUniform& aScene, Camera& camera, std::uint32_t aFramebufferWidth, std::uint32_t aFramebufferHeight);
void updateModelMatrices(const Engine::VulkanContext& aContext, glsl::ModelMatricesUniform& aModelMatrices, Engine::vk::Buffer& aBuffer, std::vector<Engine::vk::Model>& aModels, std::size_t dynamicAlignment);

extern Engine::VulkanContext vkContext;
extern Camera camera;
extern bool recreateSwapchain;
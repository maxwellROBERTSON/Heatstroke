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
#include "../Engine/Physics/PhysicsWorld.hpp"
#include "Camera.hpp"


void initialiseGame(ComponentTypeRegistry& registry, EntityManager& entityManager);
void initialiseModels(std::vector<Engine::vk::Model>& models);
void initialisePhysics(PhysicsWorld& pworld);
void runGameLoop(std::vector<Engine::vk::Model>& models, ComponentTypeRegistry& registry, EntityManager& entityManager, PhysicsWorld& pworld);


void updateSceneUniform(glsl::SceneUniform& aScene, Camera& camera, std::uint32_t aFramebufferWidth, std::uint32_t aFramebufferHeight);
void updateModelMatrices(const Engine::VulkanContext& aContext, glsl::ModelMatricesUniform& aModelMatrices, Engine::vk::Buffer& aBuffer, EntityManager& entityManager, std::size_t dynamicAlignment);
void loadOfflineEntities(ComponentTypeRegistry& registry, EntityManager& entityManager, PhysicsWorld& pworld);

extern Engine::VulkanContext vkContext;
extern Camera camera;
extern bool recreateSwapchain;
#pragma once

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Engine/Core/Game.h"

#include "../Engine/ECS/ComponentTypeRegistry.hpp"
#include "../Engine/ECS/EntityManager.hpp"
#include "../Engine/ECS/RenderComponent.hpp"
#include "../Engine/ECS/PhysicsComponent.hpp"
#include "../Engine/ECS/CameraComponent.hpp"
#include "../Engine/ECS/NetworkComponent.hpp"

#include "../Engine/Physics/PhysicsWorld.hpp"

#include "../Engine/vulkan/VulkanContext.hpp"

#include "Camera.hpp"
#include "Uniforms.hpp"

#include "../Engine/Events/Event.h"
#include "../Engine/Events/KeyEvent.h"
#include "../Engine/Events/MouseEvent.h"

class FPSTest : public Engine::Game
{
public:
	FPSTest() : Engine::Game("FPS Test Game")
	{
		this->Init();
	}
	virtual void Init() override;
	virtual void Update() override;
	virtual void OnEvent(Engine::Event& e) override;

	void registerComponents();
	void initialiseModels();
	void initialisePhysics(PhysicsWorld& pworld);
	void RenderGUI();
	void RenderScene();

	~FPSTest() {
		for (Engine::vk::Model& model : models)
			model.destroy();

		this->GetContext().allocator.reset();
		this->GetContext().window.reset();
	};

public:
	Camera* camera;
	std::vector<Engine::vk::Model> models;
	bool recreateSwapchain;
	ComponentTypeRegistry registry = ComponentTypeRegistry::Get();
	EntityManager entityManager = EntityManager(&registry);
	PhysicsWorld physics_world;

	int clientId = -1;
	bool online = false;
	bool offline = true;
};

void updateSceneUniform(glsl::SceneUniform& aScene, Camera& camera, std::uint32_t aFramebufferWidth, std::uint32_t aFramebufferHeight);
void updateModelMatrices(const Engine::VulkanContext& aContext, glsl::ModelMatricesUniform& aModelMatrices, Engine::vk::Buffer& aBuffer, EntityManager& entityManager, std::size_t dynamicAlignment);
void loadOfflineEntities(ComponentTypeRegistry& registry, EntityManager& entityManager, PhysicsWorld& pworld);
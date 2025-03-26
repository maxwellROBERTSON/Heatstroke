#pragma once

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Engine/Core/Game.hpp"

#include "../Engine/ECS/CameraComponent.hpp"
#include "../Engine/ECS/ComponentTypeRegistry.hpp"
#include "../Engine/ECS/EntityManager.hpp"
#include "../Engine/ECS/NetworkComponent.hpp"
#include "../Engine/ECS/PhysicsComponent.hpp"
#include "../Engine/ECS/RenderComponent.hpp"

#include "../Engine/Physics/PhysicsWorld.hpp"

#include "../Engine/vulkan/VulkanContext.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"
#include "../Engine/vulkan/Renderer.hpp"

#include "../Engine/Core/Camera.hpp"
#include "Uniforms.hpp"

#include "../Engine/Events/Event.hpp"
#include "../Engine/Events/KeyEvent.hpp"
#include "../Engine/Events/MouseEvent.hpp"
#include "../Engine/Events/WindowEvent.hpp"

#include "../Input/Input.hpp"
#include "../Input/InputCodes.hpp"
#include "../Input/Joystick.hpp"

class FPSTest : public Engine::Game
{
public:
	FPSTest() : Engine::Game("FPS Test Game")
	{
		this->renderer = Engine::Renderer(&this->GetContext(), &this->entityManager);
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
	};

public:
	Camera* camera;
	std::vector<Engine::vk::Model> models;
	bool recreateSwapchain;
	ComponentTypeRegistry registry = ComponentTypeRegistry::Get();
	EntityManager entityManager = EntityManager(&registry);
	Engine::Renderer renderer;
	PhysicsWorld physics_world;

	int clientId = -1;
	bool online = false;
	bool offline = true;
	bool isChange = true;
};

void loadOfflineEntities(ComponentTypeRegistry& registry, EntityManager& entityManager, PhysicsWorld& pworld, std::vector<Engine::vk::Model>& models);
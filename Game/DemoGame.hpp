#pragma once

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <chrono>

#include "../Engine/Core/Game.hpp"

#include "../Engine/ECS/EntityManager.hpp"
#include "../Engine/ECS/Components/CameraComponent.hpp"
#include "../Engine/ECS/Components/NetworkComponent.hpp"
#include "../Engine/ECS/Components/PhysicsComponent.hpp"
#include "../Engine/ECS/Components/RenderComponent.hpp"

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

#include "../ThreadPool/thread_pool_wait.h"

#include "rendering/Crosshair.hpp"

class FPSTest : public Engine::Game
{
public:
	FPSTest() : Engine::Game("FPS Test Game")
	{
		this->Init();
	}
	virtual void Init() override;
	virtual void Render() override;
	virtual void Update() override;
	virtual void OnEvent(Engine::Event& e) override;

	void initialiseModels();

	void loadOfflineEntities();
	void loadOnlineEntities(int);

	~FPSTest() {
		for (Engine::vk::Model& model : GetModels())
			model.destroy();
	};

	Crosshair& getCrosshair();

	std::chrono::steady_clock::time_point previous;
	thread_pool_wait* threadPool;
	int offlineClientId = 0;

	Crosshair crosshair;
};

//void loadOfflineEntities(ComponentTypeRegistry& registry, EntityManager& entityManager, PhysicsWorld& pworld);

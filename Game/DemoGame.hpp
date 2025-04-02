#pragma once

#include <chrono>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Engine/Core/Game.hpp"

#include "../Engine/ECS/Components/CameraComponent.hpp"
#include "../Engine/ECS/Components/NetworkComponent.hpp"
#include "../Engine/ECS/Components/PhysicsComponent.hpp"
#include "../Engine/ECS/Components/RenderComponent.hpp"
#include "../Engine/ECS/EntityManager.hpp"

#include "../Engine/Physics/PhysicsWorld.hpp"

#include "../Engine/vulkan/Renderer.hpp"
#include "../Engine/vulkan/VulkanContext.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"

#include "../Engine/Core/Camera.hpp"
#include "Uniforms.hpp"

#include "../Engine/Events/Event.hpp"
#include "../Engine/Events/KeyEvent.hpp"
#include "../Engine/Events/MouseEvent.hpp"
#include "../Engine/Events/WindowEvent.hpp"

#include "../Input/Input.hpp"
#include "../Input/InputCodes.hpp"
#include "../Input/Joystick.hpp"

#include "UserComponents/FPSCamera.hpp"
#include "UserComponents/FPSCameraComponent.hpp"

class FPSTest : public Engine::Game
{

	enum CameraMode
	{
		SCENE,
		PLAYER
	};

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
	void loadOnlineEntities();

	~FPSTest() {
		for (Engine::vk::Model& model : GetModels())
			model.destroy();
	};

	std::chrono::steady_clock::time_point previous;
	int clientId = 0;

	Engine::Camera sceneCam;
	FPSCamera playerCam;
	CameraMode camMode;
	//int currentCam = 0;
};

//void loadOfflineEntities(ComponentTypeRegistry& registry, EntityManager& entityManager, PhysicsWorld& pworld);

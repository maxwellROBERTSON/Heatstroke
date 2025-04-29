#pragma once

#include <chrono>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Engine/Core/Game.hpp"

#include "../Engine/ECS/Components/CameraComponent.hpp"
#include "../Engine/ECS/Components/NetworkComponent.hpp"
#include "../Engine/ECS/Components/PhysicsComponent.hpp"
#include "../Engine/ECS/Components/RenderComponent.hpp"
#include "../Engine/ECS/Entity.hpp"
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


#include "../ThreadPool//thread_pool_wait.h"

#include "UserComponents/FPSCamera.hpp"
#include "UserComponents/FPSCameraComponent.hpp"

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
	virtual void DrawGUI() override;

	void initialiseModels();

	void loadOfflineEntities();
	void loadOnlineEntities(int);

	~FPSTest() {
		for (Engine::vk::Model& model : GetModels())
			model.destroy();
	};

	std::chrono::steady_clock::time_point previous;

	// -- input actions
	// Cameras
	Engine::Camera sceneCam;
	Engine::CameraComponent serverCameraComponent;
	//glm::vec3 cameraOffset = glm::vec3(-0.1f, 0.5f, 0.5f); // rifle
	glm::vec3 cameraOffset = glm::vec3(0.1f, 1.3f, 0.2f); // pistol
	//glm::vec3 cameraOffset = glm::vec3(0.0f, 1.6f, -0.1f); // for character
	//glm::vec3 cameraOffset = glm::vec3();
	//glm::vec3 cameraOffset = glm::vec3(-0.1f, 0.25f, -0.15f);
	//glm::vec3 cameraOffset = glm::vec3(0.0f, 1.0f, 0.0f);
	//CameraMode camMode{ CameraMode::SCENE };
	//CameraMode camMode{ CameraMode::PLAYER }; 

	// Player 1
	Engine::Entity* playerEntity;
	//glm::vec3 playerPos{ 0.0f, 1.0f, 0.0f }; // rifle
	glm::vec3 playerPos{ 0.0f, 0.0f, 0.0f }; // pistol
	float playerXRotation{ 0.0f };
	float playerYRotation{ 0.0f };
	// Map
	Engine::Entity* mapEntity;


	// Targets (make a vector (fingers crossed) at some point)
	Engine::Entity* targetEntity1;
	Engine::Entity* targetEntity2;
	glm::vec3 target1Pos{ 2.0f, 1.0f, 0.0f }; // generate randomly
	glm::vec3 target2Pos{ -2.0f, 1.0f, 0.0f }; // ^


	// -- input actions

	thread_pool_wait* threadPool;
	int offlineClientId = 0;
};

//void loadOfflineEntities(ComponentTypeRegistry& registry, EntityManager& entityManager, PhysicsWorld& pworld);

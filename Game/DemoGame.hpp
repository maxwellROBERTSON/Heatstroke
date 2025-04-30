#pragma once

#include <chrono>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Engine/Core/Game.hpp"

#include "../Engine/ECS/Components/AudioComponent.hpp"
#include "../Engine/ECS/Components/CameraComponent.hpp"
#include "../Engine/ECS/Components/NetworkComponent.hpp"
#include "../Engine/ECS/Components/PhysicsComponent.hpp"
#include "../Engine/ECS/Components/RenderComponent.hpp"
#include "../Engine/ECS/Entity.hpp"
#include "../Engine/ECS/EntityManager.hpp"
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
	virtual void DrawGUI() override;
	virtual void DrawDebugGUI() override;


	void initialiseModels();

	void loadOfflineEntities();
	void loadOnlineEntities(int);


	~FPSTest() {
		for (Engine::vk::Model& model : GetModels())
			model.destroy();
	};

	Crosshair& getCrosshair();
	int score = 0;
	int countdown = 30;
	bool gameOver = false;
	std::chrono::steady_clock::time_point previous;

	bool showGUI{ true };

	// -- input actions
	// Cameras
	Engine::Camera sceneCam;
	Engine::CameraComponent serverCameraComponent;

	//glm::vec3 cameraOffset = glm::vec3(0.0f, 1.6f, -0.1f); // for character


	// Player 1
	// (so doesnt break) (TBD)
	Engine::Entity* playerEntity;
	glm::vec3 cameraOffset = glm::vec3(0.1f, 1.3f, 0.2f);
	glm::vec3 playerPos{ 0.0f, 0.0f, 0.0f };

	Engine::Entity* pistolEntity;
	glm::vec3 pistolCamOffset = glm::vec3(0.1f, 1.3f, 0.2f);
	glm::vec3 pistolPos{ 0.0f, 0.0f, 0.0f };

	Engine::Entity* rifleEntity;
	glm::vec3 rifleCamOffset = glm::vec3(-0.1f, 0.5f, 0.5f);
	glm::vec3 riflePos{ 0.0f, 1.0f, 0.0f };

	// Map
	Engine::Entity* mapEntity;

	// Targets (make a vector (fingers crossed) at some point)
	Engine::Entity* targetEntity;
	glm::vec3 targetPos{ 3.0f, 1.0f, 0.0f };
	//Engine::Entity* targetEntity1;
	//Engine::Entity* targetEntity2;
	//Engine::Entity* targetEntity3;
	//glm::vec3 target1Pos{ 2.0f, 1.0f, 0.0f }; // generate randomly
	//glm::vec3 target2Pos{ -2.0f, 1.0f, 0.0f }; // ^
	//glm::vec3 target3Pos{ 2.0f, 5.0f, 0.0f }; // ^

	//std::vector<Engine::Entity*> targetEntities;
	//std::vector<glm::vec3> targetPositions;


	//void RespawnTarget();


	thread_pool_wait* threadPool;
	int offlineClientId = 0;

	Crosshair crosshair;
};
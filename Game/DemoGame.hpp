#pragma once

#include <chrono>

#include "../Engine/Core/Game.hpp"

#include "../Engine/ECS/Components/AudioComponent.hpp"
#include "../Engine/ECS/Components/CameraComponent.hpp"
#include "../Engine/ECS/Components/NetworkComponent.hpp"
#include "../Engine/ECS/Components/PhysicsComponent.hpp"
#include "../Engine/ECS/Components/RenderComponent.hpp"
#include "../Engine/ECS/Entity.hpp"
#include "../Engine/ECS/EntityManager.hpp"

#include "../Engine/Physics/PhysicsWorld.hpp"

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

#include "gameRendering/Renderer.hpp"
#include "gameRendering/Crosshair.hpp"
#include "gameRendering/Decals.hpp"
#include "gameRendering/RenderMode.hpp"

#include "gameModes/GameMode.hpp"


class FPSTest : public Engine::Game
{
public:
	FPSTest() : Engine::Game("FPS Test Game") {
		this->Init();
	}

	~FPSTest() {
		for (Engine::vk::Model& model : GetModels())
			model.destroy();
	};

	virtual void Init() override;
	virtual void Render() override;
	virtual void Update() override;
	virtual void OnEvent(Engine::Event& e) override;

	void initialiseModels();

	void loadOfflineEntities();
	void loadOnlineEntities(int);

	// Setters
	void SetGameMode(std::unique_ptr<GameMode>);
	void SetRenderMode(RenderMode r) { renderMode = r; }
	
	// Getters
	Renderer& getRenderer();
	RenderMode getRenderMode();
	GameMode& GetGameMode();
	Crosshair& GetCrosshair();
	Decals& getDecals();

	std::chrono::steady_clock::time_point previous;

	// -- input actions
	// Cameras
	Engine::Camera sceneCamera;

#ifdef _DEBUG
	bool debugging = true;
#else
	bool debugging = false;
#endif

private:
	Renderer renderer;

	RenderMode renderMode = RenderMode::NO_DATA_MODE;

	std::unique_ptr<GameMode> gameMode;
	Crosshair crosshair;
	Decals decals;

	thread_pool_wait* threadPool;
	int offlineClientId = 0;
};
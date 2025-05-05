#pragma once

//#include "../../Engine/Core/Game.hpp"
//
//#include "../../Engine/ECS/Components/AudioComponent.hpp"
//#include "../../Engine/ECS/Components/CameraComponent.hpp"
//#include "../../Engine/ECS/Components/NetworkComponent.hpp"
//#include "../../Engine/ECS/Components/PhysicsComponent.hpp"
//#include "../../Engine/ECS/Components/RenderComponent.hpp"
//#include "../../Engine/ECS/Entity.hpp"
//#include "../../Engine/ECS/EntityManager.hpp"
//
//#include "../../Engine/Physics/PhysicsWorld.hpp"
//
//#include "../../Engine/vulkan/Renderer.hpp"
//#include "../../Engine/vulkan/VulkanContext.hpp"
//#include "../../Engine/vulkan/VulkanDevice.hpp"
//
//#include "../../Engine/Core/Camera.hpp"

namespace Engine {
	class Camera;
	class Entity;
}

class GameMode
{
public:
	// Constructor
	GameMode() {}

	virtual void Update(float) = 0;

	virtual void ToggleSceneCamera(Engine::Camera*) = 0;

	virtual Engine::Entity* GetPlayerEntity() { return nullptr; };

	virtual void SetPlayerEntity(Engine::Entity*) {};

	virtual void SetPistolEntity(Engine::Entity*) {};

	virtual void SetRifleEntity(Engine::Entity*) {};

	virtual void SetTargetEntity(Engine::Entity*) {};
};
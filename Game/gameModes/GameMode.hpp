#pragma once

#include "../../Engine/Core/Game.hpp"

#include "../../Engine/ECS/Components/AudioComponent.hpp"
#include "../../Engine/ECS/Components/CameraComponent.hpp"
#include "../../Engine/ECS/Components/NetworkComponent.hpp"
#include "../../Engine/ECS/Components/PhysicsComponent.hpp"
#include "../../Engine/ECS/Components/RenderComponent.hpp"
#include "../../Engine/ECS/Entity.hpp"
#include "../../Engine/ECS/EntityManager.hpp"
#include "../../Engine/ECS/EntityManager.hpp"

#include "../../Engine/Physics/PhysicsWorld.hpp"

#include "../../Engine/vulkan/Renderer.hpp"
#include "../../Engine/vulkan/VulkanContext.hpp"
#include "../../Engine/vulkan/VulkanDevice.hpp"

#include "../../Engine/Core/Camera.hpp"

using namespace Engine;

class GameMode
{
public:
	// Constructor
	GameMode() {}

	virtual void Update(Game*, float) = 0;

	virtual void ToggleSceneCamera(Game*, Camera*) = 0;

	virtual void SetPlayerEntity(Entity*) {};

	virtual void SetPistolEntity(Entity*) {};

	virtual void SetRifleEntity(Entity*) {};

	virtual void SetTargetEntity(Entity*) {};
};
#pragma once

#include <vector>
#include <glm/glm.hpp>

namespace Engine {
	class Camera;
	class Entity;
}

class GameMode
{
public:
	// Constructor
	GameMode() {}

	virtual void InitNetwork() {};

	virtual void Update(float) = 0;

	virtual void ToggleSceneCamera(Engine::Camera*) {};

	virtual Engine::Entity* GetPlayerEntity() = 0;

	virtual void SetPlayerEntity(Engine::Entity*) {};

	virtual void SetPistolEntity(Engine::Entity*) {};

	virtual void SetRifleEntity(Engine::Entity*) {};

	virtual void SetTargetEntity(Engine::Entity*) {};

	virtual glm::vec3 GetStartPos(int) = 0;

	virtual bool IsMultiPlayer() = 0;
};
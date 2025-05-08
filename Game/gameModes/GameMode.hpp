#pragma once

#include <vector>

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

	// SinglePlayer functions

	virtual void ToggleSceneCamera(Engine::Camera*) {};

	virtual Engine::Entity* GetPlayerEntity() { return nullptr; };

	virtual void SetPlayerEntity(Engine::Entity*) {};

	virtual void SetPistolEntity(Engine::Entity*) {};

	virtual void SetRifleEntity(Engine::Entity*) {};

	virtual void SetTargetEntity(Engine::Entity*) {};

	// MultiPlayer functions

	virtual void SetPlayerEntities(std::vector<Engine::Entity*>) {};

	virtual void SetPistolEntities(std::vector<Engine::Entity*>) {};

};
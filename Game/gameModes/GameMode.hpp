#pragma once

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
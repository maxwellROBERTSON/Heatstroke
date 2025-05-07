#pragma once

#include "GameMode.hpp"

class FPSTest;

class MultiPlayer : public GameMode
{
public:
	// Constructor
	MultiPlayer(FPSTest* game);

	// Getters

	// Setters
	void Update(float);

	void ToggleSceneCamera(Engine::Camera*);

	void SetPlayerEntity(Engine::Entity* e) { playerEntity = e; isPlayerCam = true; }

	void SetPistolEntity(Engine::Entity* e) { pistolEntity = e; }

	void SetRifleEntity(Engine::Entity* e) { rifleEntity = e; }

	FPSTest* game;

	Engine::Entity* playerEntity;
	Engine::Entity* pistolEntity;
	Engine::Entity* rifleEntity;
	Engine::Entity* targetEntity;

	bool isPlayerCam = false;
};
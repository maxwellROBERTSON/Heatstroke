#pragma once

#include "GameMode.hpp"

class MultiPlayer : public GameMode
{
public:
	// Constructor
	MultiPlayer() {}

	// Getters

	// Setters
	void Update(Game* game, float);

	void ToggleSceneCamera(Game*, Camera*);

	void SetPlayerEntity(Entity* e) { playerEntity = e; isPlayerCam = true; }

	void SetPistolEntity(Entity* e) { pistolEntity = e; }

	void SetRifleEntity(Entity* e) { rifleEntity = e; }

	Entity* playerEntity;
	Entity* pistolEntity;
	Entity* rifleEntity;
	Entity* targetEntity;

	bool isPlayerCam = false;
};
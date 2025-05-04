#pragma once

#include <random>

#include "GameMode.hpp"

using namespace Engine;

extern std::random_device rd;  // Declare extern random device
extern std::mt19937 gen;       // Declare extern mersenne_twister engine
extern std::uniform_int_distribution<> randomDistribX; // Declare extern distributionX
extern std::uniform_int_distribution<> randomDistribZ; // Declare extern distributionY

class SinglePlayer : public GameMode
{
public:
	// Constructor
	SinglePlayer() {}

	void Update(Game*, float) override;

	void ToggleSceneCamera(Game*, Camera*) override;

	Entity* GetPlayerEntity() override { return playerEntity; }

	void SetPlayerEntity(Game*, Entity*) override;

	void SetPistolEntity(Entity* e) override { pistolEntity = e; }

	void SetRifleEntity(Entity* e) override { rifleEntity = e; }

	void SetTargetEntity(Entity* e) override { targetEntity = e; }

	int score = 0;
	int countdown = 30;
	bool gameOver = false;
	float fireDelay = 1.0f;
	bool canFire = true;
	float counter = 1.0f;

	Entity* playerEntity = nullptr;
	Entity* pistolEntity = nullptr;
	Entity* rifleEntity = nullptr;
	Entity* targetEntity = nullptr;

	bool isPlayerCam = false;
};
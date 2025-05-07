#pragma once

#include <random>

#include "GameMode.hpp"

extern std::random_device rd;  // Declare extern random device
extern std::mt19937 gen;       // Declare extern mersenne_twister engine
extern std::uniform_int_distribution<> randomDistribX; // Declare extern distributionX
extern std::uniform_int_distribution<> randomDistribZ; // Declare extern distributionY

class FPSTest;

class SinglePlayer : public GameMode
{
public:
	// Constructor
	SinglePlayer(FPSTest* game);

	void Update(float) override;

	void ToggleSceneCamera(Engine::Camera*) override;

	Engine::Entity* GetPlayerEntity() override { return playerEntity; }

	void SetPlayerEntity(Engine::Entity*) override;

	void SetPistolEntity(Engine::Entity* e) override { pistolEntity = e; }

	void SetRifleEntity(Engine::Entity* e) override { rifleEntity = e; }

	void SetTargetEntity(Engine::Entity* e) override { targetEntity = e; }

	FPSTest* game;

	int score = 0;
	int countdown = 30;
	bool gameOver = false;
	float fireDelay = 1.5f;
	bool canFire = true;
	float counter = 1.0f;
	int ammoCount = 6;


	Engine::Entity* playerEntity = nullptr;
	Engine::Entity* pistolEntity = nullptr;
	Engine::Entity* rifleEntity = nullptr;
	Engine::Entity* targetEntity = nullptr;

	bool isPlayerCam = false;
};
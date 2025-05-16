#pragma once

#include <random>

#include "GameMode.hpp"
#include <glm/glm.hpp>

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

	//void swapCurrentGun();

	FPSTest* game;

	int score = 0;
	int countdown = 30;
	bool gameOver = false;
	float fireDelay = 0.25f;
	bool canFire = true;
	float counter = 1.0f;
	int ammoCount = 10;
	bool isReloading{ false };
	float reloadDelay = 0.5f;
	bool canReload{ true };
	bool holdingPistol{ true };

	Engine::Entity* playerEntity = nullptr;
	Engine::Entity* pistolEntity = nullptr;
	Engine::Entity* rifleEntity = nullptr;
	Engine::Entity* targetEntity = nullptr;

	// debug stuff
	glm::vec3 riflePosition{ 0.1f, -0.35f, 0.1f };
	float rifleXRot{ 0.f };
	float rifleYRot{ 180.f };
	float rifleScale{ 0.75f };

	bool isPlayerCam = false;

	void swapWeapon();
	void reloadPistol();
	void shootPistol();
	void reloadRifle();
	void shootRifle();
};
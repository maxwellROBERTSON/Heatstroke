#pragma once

#include "GameMode.hpp"

class FPSTest;

class MultiPlayer : public GameMode
{
public:
	// Constructor
	MultiPlayer(FPSTest* game);

	void InitNetwork() override;

	void Update(float) override;

	Engine::Entity* GetPlayerEntity() override { return playerEntity; }

	void SetPlayerEntity(Engine::Entity*) override;
	void SetPistolEntity(Engine::Entity* e) override { pistolEntity = e; }
	void SetRifleEntity(Engine::Entity* e) override { rifleEntity = e; }
	void SetTargetEntity(Engine::Entity* e) override { targetEntity = e; }

	glm::vec3 GetStartPos(int team) override;

	FPSTest* game;

	// Max 4 teams
	std::vector<glm::vec3> startPositions = {
		{0.0f, 0.5f, 0.0f},
		{1.0f, 0.5f, 0.0f},
		{2.0f, 0.5f, 0.0f},
		{3.0f, 0.5f, 0.0f}
	};

	int score = 0;
	int countdown = 60;
	float fireDelay = 1.5f;
	bool canFire = true;
	float counter = 1.0f;
	int ammoCount = 6;

	Engine::Entity* playerEntity = nullptr;
	Engine::Entity* pistolEntity = nullptr;
	Engine::Entity* rifleEntity = nullptr;
	Engine::Entity* targetEntity = nullptr;

};
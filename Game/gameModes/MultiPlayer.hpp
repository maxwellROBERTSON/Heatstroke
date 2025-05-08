#pragma once

#include "GameMode.hpp"

class FPSTest;

class MultiPlayer : public GameMode
{
public:
	// Constructor
	MultiPlayer(FPSTest* game);

	void Update(float);

	void SetPlayerEntities(std::vector<Engine::Entity*> e) override { playerEntities = e; };
	void SetPistolEntities(std::vector<Engine::Entity*> e) override { pistolEntities = e; };

	FPSTest* game;

	int score = 0;
	int countdown = 30;
	bool gameOver = false;
	float fireDelay = 1.0f;
	bool canFire = true;
	float counter = 1.0f;

	std::vector<Engine::Entity*> playerEntities = std::vector<Engine::Entity*>(0);
	std::vector<Engine::Entity*> pistolEntities = std::vector<Engine::Entity*>(0);
};
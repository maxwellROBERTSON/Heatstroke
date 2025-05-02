#pragma once

#include "GameMode.hpp"

class SinglePlayer : public GameMode
{
public:
	// Constructor
	SinglePlayer() {}

	// Setters
	void Update(float) override;

	Engine::Entity* PlayerEntity;
	int score = 0;
	int countdown = 30;
	bool gameOver = false;
	float fireDelay = 1.0f;
	bool canFire = true;
	float counter = 1.0f;
};
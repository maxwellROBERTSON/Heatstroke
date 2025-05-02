#pragma once

#include <random>

#include "GameMode.hpp"

using namespace Engine;

extern std::random_device rd;  // Declare extern random device
extern std::mt19937 gen;       // Declare extern mersenne_twister engine
extern std::uniform_int_distribution<> randomDistrib; // Declare extern distribution

class SinglePlayer : public GameMode
{
public:
	// Constructor
	SinglePlayer() {}

	// Setters
	void AddEntityToMap(std::string,Entity*, glm::vec3, glm::vec3);

	void Update(Game*, float);

	void ToggleSceneCamera(Game*, Camera*);

	void SetPlayerEntity(Entity* e) { playerEntity = e; isPlayerCam = true; }

	void SetPistolEntity(Entity* e) { pistolEntity = e; }

	void SetRifleEntity(Entity* e) { rifleEntity = e; }

	void SetTargetEntity(Entity* e) { targetEntity = e; }

	int score = 0;
	int countdown = 30;
	bool gameOver = false;
	float fireDelay = 1.0f;
	bool canFire = true;
	float counter = 1.0f;

	// Entity*, offset, pos
	/*std::map<std::string, std::tuple<Entity*, glm::vec3, glm::vec3>> entityMap;*/

	Entity* playerEntity;
	Entity* pistolEntity;
	Entity* rifleEntity;
	Entity* targetEntity;

	bool isPlayerCam = false;
};
#pragma once

#include "../../Engine/ECS/Entity.hpp"

class GameMode
{
public:
	// Constructor
	GameMode() {}

	virtual void Update(float) = 0;
};
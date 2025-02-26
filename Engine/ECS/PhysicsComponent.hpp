#pragma once

#include <string>

#include "Component.hpp"

class PhysicsComponent : public Component<PhysicsComponent>
{
public:
	PhysicsComponent() {};
	
	// Getters
	int GetIsPerson() { return isPerson; }

	// Setters
	void SetIsPerson(bool aIsPerson) { isPerson = aIsPerson; }

private:
	bool isPerson = false;
};



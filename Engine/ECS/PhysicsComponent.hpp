#pragma once

#include <string>

#include "Component.hpp"

class PhysicsComponent : public Component<PhysicsComponent>
{
public:
	PhysicsComponent() {};

	virtual void operator=(const PhysicsComponent& other) override
	{
		this->isPerson = other.isPerson;
	}
	
	// Getters
	int GetIsPerson() { return isPerson; }

	// Setters
	void SetIsPerson(bool aIsPerson) { isPerson = aIsPerson; }

private:
	bool isPerson = false;
};



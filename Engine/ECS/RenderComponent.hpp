#pragma once

#include <string>

#include "Component.hpp"

class RenderComponent : public Component<RenderComponent>
{
public:
	RenderComponent() {};

	virtual void operator=(const RenderComponent& other) override
	{
		this->modelIndex = other.modelIndex;
		this->isActive = other.isActive;
	}

	// Getters
	int GetModelIndex() { return modelIndex; }

	// Setters
	void SetModelIndex(int index) { modelIndex = index; }

private:
	int modelIndex = -1;
	int isActive = true;
};



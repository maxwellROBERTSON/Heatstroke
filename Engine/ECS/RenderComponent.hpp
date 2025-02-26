#pragma once

#include <string>

#include "Component.hpp"

class RenderComponent : public Component<RenderComponent>
{
public:
	RenderComponent() {};

	// Getters
	int GetModelIndex() { return modelIndex; }

	// Setters
	void SetModelIndex(int index) { modelIndex = index; }

private:
	int modelIndex = -1;
	int isActive = true;
};



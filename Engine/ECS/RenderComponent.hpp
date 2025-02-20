#pragma once

#include <string>

#include "Component.hpp"

class RenderComponent : public Component
{
public:
	RenderComponent() { meshLocation = ""; }
	RenderComponent(std::string meshLocation) : meshLocation(meshLocation) {}
	ComponentType GetType() const override { return ComponentType::Render; }

private:
	std::string meshLocation;

};


#pragma once

#include <string>

#include "Component.hpp"

class RenderComponent : public Component<RenderComponent>
{
public:
	RenderComponent() {};
private:
	std::string meshLocation = "";
};



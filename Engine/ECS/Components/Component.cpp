#include "Component.hpp"

namespace Engine
{
	std::map<ComponentTypes, uint8_t> ComponentSizes =
	{
		{ComponentTypes::CAMERA, 9},
		{ComponentTypes::NETWORK, 1},
		{ComponentTypes::PHYSICS, 23},
		{ComponentTypes::RENDER, 5}
	};
}
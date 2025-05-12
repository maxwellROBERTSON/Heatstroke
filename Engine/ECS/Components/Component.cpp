#include "Component.hpp"

namespace Engine
{
	std::map<ComponentTypes, uint8_t> ComponentSizes =
	{
		{ComponentTypes::AUDIO, 4},
		{ComponentTypes::CAMERA, 36},
		{ComponentTypes::CHILDREN, 40},
		{ComponentTypes::NETWORK, 9},
		{ComponentTypes::PHYSICS, 50},
		{ComponentTypes::RENDER, 8}
	};
}
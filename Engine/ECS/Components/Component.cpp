#include "Component.hpp"

namespace Engine
{
	std::map<ComponentTypes, uint8_t> ComponentSizes =
	{
		{ComponentTypes::AUDIO, 4},
		{ComponentTypes::CAMERA, 36},
		{ComponentTypes::CHILDREN, 4},
		{ComponentTypes::NETWORK, 8},
		{ComponentTypes::PHYSICS, 49},
		{ComponentTypes::RENDER, 8}
	};
}
#include "Component.hpp"

namespace Engine
{
	std::map<ComponentTypes, uint8_t> ComponentSizes =
	{
		{ComponentTypes::CAMERA, 36},
		{ComponentTypes::NETWORK, 8},
		{ComponentTypes::PHYSICS, 49},
		{ComponentTypes::RENDER, 8}
		//{ ComponentTypes::AUDIO, 1 }
	};
}
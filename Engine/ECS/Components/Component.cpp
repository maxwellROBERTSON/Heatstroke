#pragma once

#include "Component.hpp"

namespace Engine
{
    std::map<ComponentTypes, uint8_t> ComponentSizes =
    {
        {ComponentTypes::CAMERA, 2},
        {ComponentTypes::NETWORK, 2},
        {ComponentTypes::PHYSICS, 2},
        {ComponentTypes::RENDER, 2}
    };
}
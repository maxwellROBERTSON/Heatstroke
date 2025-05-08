#pragma once

#include "../Core/Camera.hpp"

namespace Engine
{
    class HsRenderer
    {
    public:
        virtual ~HsRenderer() = default;
    
        virtual void attachCamera(Camera* camera) = 0;
    };
}
    
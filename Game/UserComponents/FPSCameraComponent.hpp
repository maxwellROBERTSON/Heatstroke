#pragma once

#include "../ECS/Components/CameraComponent.hpp"

class FPSCameraComponent : public Engine::CameraComponent
{
public:
	void UpdateCameraPosition(glm::vec3 pos);
};


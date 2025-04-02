#pragma once

#include "../../ECS/CameraComponent.hpp"
class FPSCameraComponent : public CameraComponent
{
public:
	void UpdateCameraPosition(glm::vec3 pos);
};


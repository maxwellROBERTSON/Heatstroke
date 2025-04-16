#pragma once

#include "../ECS/Components/CameraComponent.hpp"

class FPSCameraComponent : public Engine::CameraComponent
{
public:
	void UpdateCameraPosition(glm::vec3 pos);
	void SetCameraOffset(float x, float y, float z);
	void SetCameraOffset(glm::vec3 offset);

	void setRotationEnabled(bool aBool);
	glm::vec3 GetFrontDirection();
	//glm::vec3 GetCameraOffset() const { return cameraOffset; }
	glm::vec3 cameraOffset;

};


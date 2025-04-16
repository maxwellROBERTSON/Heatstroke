#include "FPSCameraComponent.hpp"

#include <Engine/Core/Game.hpp>
#include <GLFW/glfw3.h>


void FPSCameraComponent::UpdateCameraPosition(glm::vec3 pos)
{
	//this->GetCamera()->position = pos;
	//this->GetCamera()->position = pos + cameraOffset;
	glm::vec3 forwardOffset = glm::normalize(this->GetCamera()->frontDirection) * cameraOffset.z;
	glm::vec3 upOffset = glm::vec3(0.0f, 1.0f, 0.0f) * cameraOffset.y;
	glm::vec3 rightOffset = glm::normalize(glm::cross(this->GetCamera()->frontDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * cameraOffset.x;
	glm::vec3 offset = forwardOffset + upOffset + rightOffset;
	this->GetCamera()->position = pos + offset;
}

void FPSCameraComponent::SetCameraOffset(float x, float y, float z)
{
	cameraOffset = glm::vec3(x, y, z);
}

void FPSCameraComponent::SetCameraOffset(glm::vec3 offset)
{
	cameraOffset = offset;
}

void FPSCameraComponent::setRotationEnabled(bool aBool)
{
	this->GetCamera()->enableRotation = aBool;
	if (this->GetCamera()->enableRotation)
	{
		this->GetCamera()->lastX = (float)1280 / 2;
		this->GetCamera()->lastY = (float)720 / 2;
	}
}

glm::vec3 FPSCameraComponent::GetFrontDirection()
{
	return this->GetCamera()->frontDirection;
}

#include "FPSCameraComponent.hpp"

void FPSCameraComponent::UpdateCameraPosition(glm::vec3 pos)
{
	this->GetCamera()->position = pos;
}

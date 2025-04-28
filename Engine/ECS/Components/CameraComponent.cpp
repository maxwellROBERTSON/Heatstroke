#include "CameraComponent.hpp"

#include <cstring>

namespace Engine
{
	class Camera;
}

namespace Engine
{
	// Getters
// Get component data

	void CameraComponent::swapCameraMode()
	{
		switch (this->GetCamera()->camMode)
		{
		case CameraMode::SCENE:
			this->GetCamera()->camMode = CameraMode::PLAYER;
			this->GetCamera()->position = playerEntity->GetPosition();
			//this->GetCamera()->position = playerEntity->GetPosition() + glm::vec3(0.0f, 1.0f, 0.0f);
			break;
		case CameraMode::PLAYER:
			this->GetCamera()->camMode = CameraMode::SCENE;
			//this->GetCamera()->position = sceneCamPos;
			break;
		default:
			this->GetCamera()->camMode = CameraMode::SCENE;
			//this->GetCamera()->position = sceneCamPos;
			break;
		}
	}

	void CameraComponent::GetDataArray(uint8_t* data)
	{
		size_t offset = 0;
		std::memcpy(data + offset, &camera.fov, sizeof(camera.fov));
		offset += sizeof(camera.fov);
		std::memcpy(data + offset, &camera.nearPlane, sizeof(camera.nearPlane));
		offset += sizeof(camera.nearPlane);
		std::memcpy(data + offset, &camera.farPlane, sizeof(camera.farPlane));
		offset += sizeof(camera.farPlane);
		std::memcpy(data + offset, &camera.position, sizeof(camera.position));
		offset += sizeof(camera.position);
		std::memcpy(data + offset, &camera.frontDirection, sizeof(camera.frontDirection));
	}
	void CameraComponent::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		if (std::memcmp(&camera.fov, data + offset, sizeof(camera.fov)) != 0)
		{
			std::memcpy(&camera.fov, data + offset, sizeof(camera.fov));
			SetComponentHasChanged();
		}
		offset += sizeof(camera.fov);

		if (std::memcmp(&camera.nearPlane, data + offset, sizeof(camera.nearPlane)) != 0)
		{
			std::memcpy(&camera.nearPlane, data + offset, sizeof(camera.nearPlane));
			SetComponentHasChanged();
		}
		offset += sizeof(camera.nearPlane);

		if (std::memcmp(&camera.farPlane, data + offset, sizeof(camera.farPlane)) != 0)
		{
			std::memcpy(&camera.farPlane, data + offset, sizeof(camera.farPlane));
			SetComponentHasChanged();
		}
		offset += sizeof(camera.farPlane);

		if (std::memcmp(&camera.position, data + offset, sizeof(camera.position)) != 0)
		{
			std::memcpy(&camera.position, data + offset, sizeof(camera.position));
			SetComponentHasChanged();
		}
		offset += sizeof(camera.position);

		if (std::memcmp(&camera.frontDirection, data + offset, sizeof(camera.frontDirection)) != 0)
		{
			std::memcpy(&camera.frontDirection, data + offset, sizeof(camera.frontDirection));
			SetComponentHasChanged();
		}
	}


	// Update camera using Camera class
	void CameraComponent::UpdateCamera(GLFWwindow* aWindow, float timeDelta)
	{
		Camera tempCamera = camera;
		camera.updateCamera(aWindow, timeDelta);
		if (!(camera == tempCamera))
			SetComponentHasChanged();
	}

	void CameraComponent::UpdateCameraPosition(glm::vec3 pos)
	{
		glm::vec3 forwardOffset = glm::normalize(this->GetCamera()->frontDirection) * cameraOffset.z;
		glm::vec3 upOffset = glm::vec3(0.0f, 1.0f, 0.0f) * cameraOffset.y;
		glm::vec3 rightOffset = glm::normalize(glm::cross(this->GetCamera()->frontDirection, glm::vec3(0.0f, 1.0f, 0.0f))) * cameraOffset.x;
		glm::vec3 offset = forwardOffset + upOffset + rightOffset;
		this->GetCamera()->position = pos + offset;
	}

	// Set component has changed in entity manager
	void CameraComponent::SetComponentHasChanged()
	{
		if (!hasChanged)
		{
			entityManager->AddChangedComponent(StaticType(), entity);
			hasChanged = true;
		}
	}
	void CameraComponent::SetCameraOffset(float x, float y, float z)
	{
		cameraOffset = glm::vec3(x, y, z);
	}
	void CameraComponent::SetCameraOffset(glm::vec3 offset)
	{
		cameraOffset = offset;
	}
	glm::vec3 CameraComponent::GetFrontDirection()
	{
		return this->GetCamera()->frontDirection;
	}
}

// // Get component data
// void CameraComponent::GetDataArray(uint8_t* data)
// {
// 	size_t offset = 0;

// 	Engine::Camera temp = *camera;
// 	std::memcpy(data + offset, &temp.fov, sizeof(temp.fov));
// 	offset += sizeof(temp.fov);
// 	std::memcpy(data + offset, &temp.nearPlane, sizeof(temp.nearPlane));
// 	offset += sizeof(temp.nearPlane);
// 	std::memcpy(data + offset, &temp.farPlane, sizeof(temp.farPlane));
// 	offset += sizeof(temp.farPlane);
// 	std::memcpy(data + offset, &temp.position, sizeof(temp.position));
// 	offset += sizeof(temp.position);
// 	std::memcpy(data + offset, &temp.frontDirection, sizeof(temp.frontDirection));
// }

// // Setters

// // Set component data
// void CameraComponent::SetDataArray(uint8_t* data)
// {
// 	size_t offset = 0;

// 	Engine::Camera temp = *camera;

// 	std::memcpy(&temp.fov, data + offset, sizeof(temp.fov));
// 	offset += sizeof(temp.fov);
// 	std::memcpy(&temp.nearPlane, data + offset, sizeof(temp.nearPlane));
// 	offset += sizeof(temp.nearPlane);
// 	std::memcpy(&temp.farPlane, data + offset, sizeof(temp.farPlane));
// 	offset += sizeof(temp.farPlane);
// 	std::memcpy(&temp.position, data + offset, sizeof(temp.position));
// 	offset += sizeof(temp.position);
// 	std::memcpy(&temp.frontDirection, data + offset, sizeof(temp.frontDirection));
// }
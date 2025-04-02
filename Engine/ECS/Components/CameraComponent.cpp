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
	void CameraComponent::GetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		Engine::Camera temp = *camera;
		std::memcpy(data + offset, &temp.fov, sizeof(temp.fov));
		offset += sizeof(temp.fov);
		std::memcpy(data + offset, &temp.nearPlane, sizeof(temp.nearPlane));
		offset += sizeof(temp.nearPlane);
		std::memcpy(data + offset, &temp.farPlane, sizeof(temp.farPlane));
		offset += sizeof(temp.farPlane);
		std::memcpy(data + offset, &temp.position, sizeof(temp.position));
		offset += sizeof(temp.position);
		std::memcpy(data + offset, &temp.frontDirection, sizeof(temp.frontDirection));
	}

	// Setters

	// Set component data
	void CameraComponent::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		Engine::Camera temp = *camera;

		std::memcpy(&temp.fov, data + offset, sizeof(temp.fov));
		offset += sizeof(temp.fov);
		std::memcpy(&temp.nearPlane, data + offset, sizeof(temp.nearPlane));
		offset += sizeof(temp.nearPlane);
		std::memcpy(&temp.farPlane, data + offset, sizeof(temp.farPlane));
		offset += sizeof(temp.farPlane);
		std::memcpy(&temp.position, data + offset, sizeof(temp.position));
		offset += sizeof(temp.position);
		std::memcpy(&temp.frontDirection, data + offset, sizeof(temp.frontDirection));
	}
}
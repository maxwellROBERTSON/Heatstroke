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

	// Setters

	// Set component data
	void CameraComponent::SetDataArray(uint8_t* data)
	{
        size_t offset = 0;

        std::memcpy(&camera.fov, data + offset, sizeof(camera.fov));
        offset += sizeof(camera.fov);
        std::memcpy(&camera.nearPlane, data + offset, sizeof(camera.nearPlane));
        offset += sizeof(camera.nearPlane);
        std::memcpy(&camera.farPlane, data + offset, sizeof(camera.farPlane));
        offset += sizeof(camera.farPlane);
        std::memcpy(&camera.position, data + offset, sizeof(camera.position));
        offset += sizeof(camera.position);
        std::memcpy(&camera.frontDirection, data + offset, sizeof(camera.frontDirection));
	}
}
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

    // Set component has changed in entity manager
    void CameraComponent::SetComponentHasChanged()
    {
        if (!hasChanged)
        {
            entityManager->AddChangedComponent(StaticType(), entity);
            hasChanged = true;
        }
    }
}
#pragma once

#include "CameraComponent.hpp"

namespace Engine
{
	class Camera;
}

namespace Engine
{
	// Getters

	// Get component data
	void CameraComponent::GetDataArray(std::vector<uint8_t>& data)
	{
		uint8_t* rawData = reinterpret_cast<uint8_t*>(&camera.fov);
		data.insert(data.end(), rawData, rawData + sizeof(camera.fov));
		rawData = reinterpret_cast<uint8_t*>(&camera.nearPlane);
		data.insert(data.end(), rawData, rawData + sizeof(camera.nearPlane));
		rawData = reinterpret_cast<uint8_t*>(&camera.farPlane);
		data.insert(data.end(), rawData, rawData + sizeof(camera.farPlane));
		rawData = reinterpret_cast<uint8_t*>(&camera.position);
		data.insert(data.end(), rawData, rawData + sizeof(camera.position));
		rawData = reinterpret_cast<uint8_t*>(&camera.frontDirection);
		data.insert(data.end(), rawData, rawData + sizeof(camera.frontDirection));
	}

	// Setters

	// Set component data
	void CameraComponent::SetDataArray(uint8_t* data)
	{
		;
	}
}
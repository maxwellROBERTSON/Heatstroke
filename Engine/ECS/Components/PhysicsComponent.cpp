#pragma once

#include "PhysicsComponent.hpp"

namespace Engine
{
	class PhysicsWorld;

	// Getters

	void PhysicsComponent::GetDataArray(std::vector<uint8_t>& data)
	{
		uint8_t* rawData = reinterpret_cast<uint8_t*>(&type);
		data.insert(data.end(), rawData, rawData + sizeof(type));
		rawData = reinterpret_cast<uint8_t*>(&translation);
		data.insert(data.end(), rawData, rawData + sizeof(translation));
		rawData = reinterpret_cast<uint8_t*>(&scale);
		data.insert(data.end(), rawData, rawData + sizeof(scale));
		rawData = reinterpret_cast<uint8_t*>(&rotation);
		data.insert(data.end(), rawData, rawData + sizeof(rotation));
	}

	// Setters

	void PhysicsComponent::SetDataArray(uint8_t* data)
	{
		;
	}
}
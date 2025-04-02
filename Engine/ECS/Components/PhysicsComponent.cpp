#pragma once

#include "PhysicsComponent.hpp"

namespace Engine
{
	class PhysicsWorld;

	// Getters

	void PhysicsComponent::GetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(data + offset, &type, sizeof(type));
		offset += sizeof(type);
		std::memcpy(data + offset, &translation, sizeof(translation));
		offset += sizeof(translation);
		std::memcpy(data + offset, &scale, sizeof(scale));
		offset += sizeof(scale);
		std::memcpy(data + offset, &rotation, sizeof(rotation));
		offset += sizeof(rotation);
		std::memcpy(data + offset, &isPerson, sizeof(isPerson));
		offset += sizeof(isPerson);
		std::memcpy(data + offset, &entityId, sizeof(entityId));
	}

	// Setters

	void PhysicsComponent::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy( &type, data + offset, sizeof(type));
		offset += sizeof(type);
		std::memcpy(&translation, data + offset, sizeof(translation));
		offset += sizeof(translation);
		std::memcpy(&scale, data + offset, sizeof(scale));
		offset += sizeof(scale);
		std::memcpy(&rotation, data + offset, sizeof(rotation));
		offset += sizeof(rotation);
		std::memcpy(&isPerson, data + offset, sizeof(isPerson));
		offset += sizeof(isPerson);
		std::memcpy(&entityId, data + offset, sizeof(entityId));
	}
}
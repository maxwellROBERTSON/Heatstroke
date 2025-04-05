#pragma once

#include "RenderComponent.hpp"

namespace Engine
{
	// Getters

	// Get component data
	void RenderComponent::GetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(data + offset, &modelIndex, sizeof(modelIndex));
		offset += sizeof(modelIndex);
		std::memcpy(data + offset, &isActive, sizeof(isActive));
	}

	// Setters

	// Set component data
	void RenderComponent::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(&modelIndex, data + offset, sizeof(modelIndex));
		offset += sizeof(modelIndex);
		std::memcpy(&isActive, data + offset, sizeof(isActive));
	}

	// Set component has changed in entity manager
	void RenderComponent::SetComponentHasChanged()
	{
		if (!hasChanged)
		{
			entityManager->AddChangedComponent(StaticType(), entity);
			hasChanged = true;
		}
	}
}
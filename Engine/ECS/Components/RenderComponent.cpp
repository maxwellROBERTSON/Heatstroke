#include "RenderComponent.hpp"

#include <cstring>

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

		if (std::memcmp(&modelIndex, data + offset, sizeof(modelIndex)) != 0)
		{
			std::memcpy(&modelIndex, data + offset, sizeof(modelIndex));
			SetComponentHasChanged();
		}
		offset += sizeof(modelIndex);

		if (std::memcmp(&isActive, data + offset, sizeof(isActive)) != 0)
		{
			std::memcpy(&isActive, data + offset, sizeof(isActive));
			SetComponentHasChanged();
		}
		offset += sizeof(isActive);
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
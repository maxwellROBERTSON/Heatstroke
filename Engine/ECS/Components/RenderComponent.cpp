#include "RenderComponent.hpp"

#include <cstring>

namespace Engine
{
	// Getters

	void RenderComponent::GetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(data + offset, &modelIndex, sizeof(modelIndex));
		offset += sizeof(modelIndex);
		std::memcpy(data + offset, &isActive, sizeof(isActive));
	}

	// Setters

	void RenderComponent::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(&modelIndex, data + offset, sizeof(modelIndex));
		offset += sizeof(modelIndex);
		std::memcpy(&isActive, data + offset, sizeof(isActive));
	}
}
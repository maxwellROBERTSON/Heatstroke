#pragma once

#include "RenderComponent.hpp"

namespace Engine
{
	// Getters

	void RenderComponent::GetDataArray(std::vector<uint8_t>& data)
	{
		uint8_t* rawData = reinterpret_cast<uint8_t*>(&modelIndex);
		data.insert(data.end(), rawData, rawData + sizeof(modelIndex));
		rawData = reinterpret_cast<uint8_t*>(&isActive);
		data.insert(data.end(), rawData, rawData + sizeof(isActive));
	}

	// Setters

	void RenderComponent::SetDataArray(uint8_t* data)
	{
		;
	}
}
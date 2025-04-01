#pragma once

#include "NetworkComponent.hpp"

namespace Engine
{
	// Getters

	void NetworkComponent::GetDataArray(std::vector<uint8_t>& data)
	{
		uint8_t* rawData = reinterpret_cast<uint8_t*>(&clientId);
		data.insert(data.end(), rawData, rawData + sizeof(clientId));
	}

	// Setters

	void NetworkComponent::SetDataArray(uint8_t* data)
	{
		;
	}
}
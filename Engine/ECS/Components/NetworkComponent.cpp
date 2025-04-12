#include "NetworkComponent.hpp"

#include <cstring>

namespace Engine
{
	// Getters

	void NetworkComponent::GetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(data + offset, &clientId, sizeof(clientId));
	}

	// Setters

	void NetworkComponent::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(&clientId, data + offset, sizeof(clientId));
	}
}
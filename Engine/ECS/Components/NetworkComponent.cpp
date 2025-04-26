#include "NetworkComponent.hpp"

#include <cstring>

namespace Engine
{
	// Getters

	// Get component data
	void NetworkComponent::GetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(data + offset, &clientId, sizeof(clientId));
		offset += sizeof(clientId);
	}

	// Setters

	// Set component data
	void NetworkComponent::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;
		if (std::memcmp(&clientId, data + offset, sizeof(clientId)) != 0)
		{
			std::memcpy(&clientId, data + offset, sizeof(clientId));
			SetComponentHasChanged();
		}
		offset += sizeof(clientId);
	}


	// Set component has changed in entity manager
	void NetworkComponent::SetComponentHasChanged()
	{
		if (!hasChanged)
		{
			entityManager->AddChangedComponent(StaticType(), entity);
			hasChanged = true;
		}
	}
}
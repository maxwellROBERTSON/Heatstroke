#pragma once

#include <string>

#include "Component.hpp"

namespace Engine
{

	class NetworkComponent : public Component<NetworkComponent>
	{
	public:
		NetworkComponent() {};

		void operator=(const NetworkComponent& other) override
		{
			this->clientId = other.clientId;
		}

		// Getters

		// Static type getter from Component parent
		ComponentTypes static StaticType() { return ComponentTypes::NETWORK; }

		// Static size getter from Component parent
		size_t static StaticSize() { return sizeof(clientId); }

		// Get component data
		void GetDataArray(uint8_t*) override;

		// Get client id
		uint64_t GetClientId() { return clientId; };

		// Setters

		// Set component data
		void SetDataArray(uint8_t*) override;

		// Set client id
		void SetClientId(uint64_t aClientId) { clientId = aClientId; }

	private:
		uint64_t clientId = 0;
	};
}

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

		// Get component data
		void GetDataArray(uint8_t*) override;

		// Get client id
		int GetClientId() { return clientId; };

		// Setters

		// Set component data
		void SetDataArray(uint8_t*) override;

		// Set client id
		void SetClientId(int aClientId) { clientId = aClientId; }

	private:
		int clientId = -1;
	};
}

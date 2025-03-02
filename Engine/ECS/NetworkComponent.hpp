#pragma once

#include <string>

#include "Component.hpp"

class NetworkComponent : public Component<NetworkComponent>
{
public:
	NetworkComponent() {};

	virtual void operator=(const NetworkComponent& other) override
	{
		this->clientId = other.clientId;
	}

	// Getters
	int GetClientId() { return clientId; };

	// Setters
	void SetClientId(int aClientId) { clientId = aClientId; }

private:
	int clientId = -1;
};



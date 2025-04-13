#pragma once

#include <string>

#include "Component.hpp"

#include "../EntityManager.hpp"

namespace Engine
{
	class EntityManager;
	class Entity;
}

namespace Engine
{

	class NetworkComponent : public Component<NetworkComponent>
	{
	public:
		NetworkComponent() : entityManager(nullptr), entity(nullptr) {}
		NetworkComponent(Engine::EntityManager* entityManager, Engine::Entity* entity) : entityManager(entityManager), entity(entity) {}

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
		void SetClientId(uint64_t aClientId) { clientId = aClientId; SetComponentHasChanged(); }

		// Set component has changed in entity manager
		void SetComponentHasChanged();

		// Toggle has changed boolean
		void ToggleHasChanged() { hasChanged = !hasChanged; }

	private:
		// EntityManager pointer
		Engine::EntityManager* entityManager;
		// Entity pointer
		Engine::Entity* entity;

		// If component has changed since last network update
		bool hasChanged = false;

		uint64_t clientId = -1;
	};
}

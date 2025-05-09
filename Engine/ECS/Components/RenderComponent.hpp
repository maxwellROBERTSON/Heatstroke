#pragma once

#include <string>

#include "Component.hpp"

#include "../EntityManager.hpp"

#include "PhysicsComponent.hpp"

namespace Engine
{
	class EntityManager;
	class Entity;

	class RenderComponent : public Component<RenderComponent>
	{
	public:
		RenderComponent() : entityManager(nullptr), entity(nullptr) {}
		RenderComponent(Engine::EntityManager* entityManager, Engine::Entity* entity) : entityManager(entityManager), entity(entity) {}

		virtual void operator=(const RenderComponent& other) override
		{
			this->modelIndex = other.modelIndex;
			this->isActive = other.isActive;
		}

		// Getters

		// Type getter from Component parent
		ComponentTypes StaticType() const override { return ComponentTypes::RENDER; }

		// Size getter from Component parent
		size_t StaticSize() const override { return sizeof(modelIndex) + sizeof(isActive); }

		// Get component data
		void GetDataArray(uint8_t*) override;

		// Get model index
		int GetModelIndex() { return modelIndex; }

		// Get is the entity an active render component
		bool GetIsActive() { return isActive; }

		// Setters

		// Set component data
		void SetDataArray(uint8_t*) override;

		// Set model index
		void SetModelIndex(int index) { modelIndex = index; SetComponentHasChanged(); }

		// Set is the entity an active render component
		void SetIsActive(bool status) { isActive = status; SetComponentHasChanged(); }

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

		int modelIndex = -1;
		int isActive = 1;
	};
}

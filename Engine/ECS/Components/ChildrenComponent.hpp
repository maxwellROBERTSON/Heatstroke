#pragma once

#include "Component.hpp"

#include "../EntityManager.hpp"

namespace Engine
{
	class ChildrenComponent : public Component<ChildrenComponent>
	{
	public:
		ChildrenComponent() : entityManager(nullptr), entity(nullptr) {}
		ChildrenComponent(EntityManager* entityManager, Entity* entity) : entityManager(entityManager), entity(entity) {}
		
		void operator=(const ChildrenComponent& other) override
		{
			childrenEntityIds = other.childrenEntityIds;
		}

		// Getters
		
		// Type getter from Component parent
		ComponentTypes StaticType() const override { return ComponentTypes::CHILDREN; }

		// Size getter from Component parent
		size_t StaticSize() const override { return sizeof(childrenEntityIds); }
		
		// Get component data
		void GetDataArray(uint8_t*) override;

		// Get number of children
		int GetNumChildren() { return numChildren; }

		// Get entityIds of children
		std::vector<int> GetChildrenEntityIds();
		
		// Setters
		
		// Set component data
		void SetDataArray(uint8_t*) override;

		// Set component has changed in entity manager
		void SetComponentHasChanged();

		// Toggle has changed boolean
		void ToggleHasChanged() { hasChanged = !hasChanged; }

		// Add a child of this entity
		void AddChild(int);

	private:
		// EntityManager pointer
		EntityManager* entityManager;
		// Entity pointer
		Entity* entity;

		int numChildren = 0;
		std::vector<int> childrenEntityIds = std::vector<int>(10, -1);

		// If component has changed since last network update
		bool hasChanged = false;
	};
}

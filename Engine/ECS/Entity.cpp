#include <iostream>

#include "Entity.hpp"
#include "EntityManager.hpp"

namespace Engine
{
	class EntityManager;

	// Constructors

	Entity::Entity(EntityManager* entityManager)
		: entityManager(entityManager), entityId(-1)
	{
	}

	Entity::Entity(EntityManager* entityManager, int entityId, std::vector<int> indexList)
		: entityManager(entityManager), entityId(entityId)
	{
		// Add index list to the entity
		componentTypeIndexList = indexList;
	}

	// Getters

	// Get the data for a given entity
	void Entity::GetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(data + offset, &entityId, sizeof(entityId));
		offset += sizeof(entityId);
		std::memcpy(data + offset, &modelMatrix, sizeof(modelMatrix));
	}

	// Setters

	// Set the data for a given entity
	void Entity::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(&entityId, data + offset, sizeof(entityId));
		offset += sizeof(entityId);
		std::memcpy(&modelMatrix, data + offset, sizeof(modelMatrix));
	}

	// Set component has changed in entity manager
	void Entity::SetEntityHasChanged()
	{
		if (!hasChanged)
		{
			entityManager->AddChangedEntity(this);
			hasChanged = true;
		}
	}
}

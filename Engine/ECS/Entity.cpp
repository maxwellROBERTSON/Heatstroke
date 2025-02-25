#include <iostream>

#include "Entity.hpp"

// Public Methods

Entity::Entity(EntityManager* entityManager, int entityId, std::vector<int> typeIndexList)
	: entityManager(entityManager), entityId(entityId)
{
	// Add all components to the entity
	for (int i = 0; i < typeIndexList.size(); i++)
	{
		componentListId.push_back(typeIndexList[i]);
	}
}

int Entity::HasComponent(int id)
{
	return componentListId[id];
}

int Entity::GetComponentIndexArray()
{
	return static_cast<int>(componentListId.size());
}

// Private Methods

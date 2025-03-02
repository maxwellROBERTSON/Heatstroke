#include "EntityManager.hpp"

// Public Methods

int EntityManager::GetNumberOfEntities()
{
	return static_cast<int>(entities.size());
}

void EntityManager::SetComponentTypesPointers(std::vector<std::pair<void*, int>> componentTypePointers)
{
	componentList = componentTypePointers;
}

void EntityManager::SetEntitiesWithType()
{
	entitiesWithType = std::vector<std::vector<int>>(registry->GetNumberOfComponentTypes());
}

// Private Methods

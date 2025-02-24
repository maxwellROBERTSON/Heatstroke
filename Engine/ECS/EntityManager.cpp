#include "EntityManager.hpp"

// Public Methods

void EntityManager::AddComponentTypesPointers(std::vector<std::pair<void*, int>> componentTypePointers)
{
	componentList = componentTypePointers;
}

int EntityManager::GetNumberOfEntities()
{
	return static_cast<int>(entities.size());
}

// Private Methods

#include "EntityManager.hpp"

// Public Methods

int EntityManager::GetNumberOfEntities()
{
	return static_cast<int>(entities.size());
}

void EntityManager::GetData(uint8_t* block)
{
	// # entites
	// bits on off for components
	// entity count, numComps(0 or 1), data[]
	block = new uint8_t[1000];
	size_t numEntities = entities.size();
	block[0] = numEntities;
	for (size_t i = 0; i < numEntities; i++)
	{
		block[i + 1]
	}
	block[1] |= (1 << bitPosition);
	block[1] = registry->GetNumberOfComponentTypes();
}

void EntityManager::SetComponentTypesPointers(std::vector<std::pair<void*, int>> componentTypePointers)
{
	componentList = componentTypePointers;
}

void EntityManager::SetEntitiesWithType()
{
	entitiesWithType = std::vector<std::vector<int>>(registry->GetNumberOfComponentTypes());
}

void EntityManager::ClearManager()
{
	entities.clear();
	for (int i = 0; i < entitiesWithType.size(); i++)
	{
		entitiesWithType[i] = std::vector<int>();
		delete[] componentList[i].first;
		componentList[i].first = nullptr;
		componentList[i].second = 0;
	}
}

// Private Methods

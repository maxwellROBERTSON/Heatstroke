#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Entity.hpp"
#include "Component.hpp"
#include "ComponentTypeRegistry.hpp"

class EntityManager
{
public:
	EntityManager(ComponentTypeRegistry* registry) : registry(registry) {};
	~EntityManager() {};

	// Getters
	int GetNumberOfEntities();
	template <typename T>
	T* GetEntityComponent(int entityId)
	{
		int typeIndex = registry->GetComponentID<T>();
		int componentId = entities[entityId].GetComponent(typeIndex);
		if (componentId == -1)
		{
			return nullptr;
		}
		else
		{
			return static_cast<T*>(componentList[typeIndex].first) + componentId * sizeof(T);
		}
	}
	Entity* GetEntity(int entityId)
	{
		return &entities[entityId];
	}

	// Setters
	void SetComponentTypesPointers(std::vector<std::pair<void*, int>>);
	template <typename... Types>
	Entity* AddEntity()
	{
		int numberOfComponentTypes = registry->GetNumberOfComponentTypes();

		std::vector<int> typeIndexList(numberOfComponentTypes, -1);
		(addComponent<Types>(typeIndexList), ...);

		Entity entity = Entity(this, entities.size(), typeIndexList);
		entities.push_back(entity);
		return &entities[entities.size() - 1];
	}

private:
	ComponentTypeRegistry* registry;
	std::vector<Entity> entities;
	std::vector<std::pair<void*, int>> componentList;

	int localPlayerEntityId = -1;

	template <typename T>
	void addComponent(std::vector<int>& typeIndexList)
	{
		int typeIndex = registry->GetComponentID<T>();
		typeIndexList[typeIndex] = static_cast<int>(componentList[typeIndex].second);
		T* newMemory = new T[componentList[typeIndex].second + 1];
		T* oldMemory = static_cast<T*>(componentList[typeIndex].first);

		for (int i = 0; i < componentList[typeIndex].second; i++)
		{
			newMemory[i] = oldMemory[i];
		}

		delete[] oldMemory;

		componentList[typeIndex].first = static_cast<void*>(newMemory);
		componentList[typeIndex].second++;
	}
};

#pragma once

#include <vector>
#include <string>
#include <memory>

#include "Entity.hpp"
#include "Component.hpp"
#include "ComponentTypeRegistry.hpp"
#include "RenderComponent.hpp"

class EntityManager
{
public:
	EntityManager(ComponentTypeRegistry* registry) : registry(registry) {};
	~EntityManager()
	{
		for (int i = 0; i < componentList.size(); i++)
		{
			delete[] componentList[i].first;
		}
	}

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
			return static_cast<T*>(componentList[typeIndex].first) + componentId;
		}
	}
	Entity* GetEntity(int entityId)
	{
		return &entities[entityId];
	}
	template <typename T>
	int GetComponentTypeSize()
	{
		int typeIndex = registry->GetComponentID<T>();
		return componentList[typeIndex].second;
	}
	template <typename T>
	std::pair<void*, int> GetComponents()
	{
		int typeIndex = registry->GetComponentID<T>();
		return componentList[typeIndex];
	}
	template <typename T>
	std::vector<int> GetEntitiesWithComponent()
	{
		int typeIndex = registry->GetComponentID<T>();
		return entitiesWithType[typeIndex];
	}
	void GetData(uint8_t* block);

	// Setters
	void SetComponentTypesPointers(std::vector<std::pair<void*, int>>);
	void SetEntitiesWithType();
	template <typename... Types>
	Entity* AddEntity()
	{
		int numberOfComponentTypes = registry->GetNumberOfComponentTypes();

		std::vector<int> typeIndexList(numberOfComponentTypes, -1);
		(addComponent<Types>(typeIndexList), ...);

		Entity entity = Entity(this, entities.size(), typeIndexList);
		entities.push_back(entity);

		for (int i = 0; i < numberOfComponentTypes; i++)
		{
			if (typeIndexList[i] != -1)
			{
				entitiesWithType[i].push_back(entities.size() - 1);
			}
		}

		return &entities[entities.size() - 1];
	}
	void ClearManager();

private:
	ComponentTypeRegistry* registry;
	std::vector<Entity> entities;
	std::vector<std::vector<int>> entitiesWithType;
	std::vector<std::pair<void*, int>> componentList;

	// For each entity:
	// Component vector of int = registry.GetNumComponents * sizeof(int)
	// Matrix = 16 * sizeof(float)
	// If rendercomp:
	//  Model index = sizeof(int)
	// If camera:
	// fov + near + far + vec3(pos) + vec3(front_dir) = 9 * float
	// If network:
	// clientid = int
	int sizeofEntities = entities.size() * (25 * sizeof(float) + sizeof(int) + 

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

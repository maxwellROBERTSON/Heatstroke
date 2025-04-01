#include <algorithm>

#include "EntityManager.hpp"

namespace Engine
{
	// Getters

	// Get total data size
	size_t EntityManager::GetTotalDataSize()
	{
		if (entities.size() == 0)
			return 0;

		size_t totalSize = (size_t)GetNumberOfEntities() * entities[0].GetEntitySize();
		ComponentTypes type;
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			type = static_cast<ComponentTypes>(i);
			totalSize += (*componentMap[type]).size() * ComponentSizes[type];
		}
	}

	// Get a pointer to the component of an entity
	ComponentBase* EntityManager::GetComponentOfEntity(int entityId, ComponentTypes type)
	{
		int index = entities[entityId].GetComponent(type);
		if (index == -1)
		{
			return nullptr;
		}
		else
		{
			return (*componentMap[type])[index].get();
		}
	}

	// Get component data of all entities
	std::vector<uint8_t> EntityManager::GetComponentData(ComponentTypes type)
	{
		std::vector<uint8_t> data = std::vector<uint8_t>(ComponentSizes[type] * (*componentMap[type]).size());
		std::vector<uint8_t> temp;
		for (int i = 0; i < (*componentMap[type]).size(); i++)
		{
			(*componentMap[type])[i]->GetDataArray(data);
		}
		return data;
	}

	//void EntityManager::GetData(uint8_t* block)
	//{
	//	// # entites
	//	// bits on off for components
	//	// entity count, numComps(0 or 1), data[]
	//	block = new uint8_t[1000];
	//	size_t numEntities = entities.size();
	//	block[0] = numEntities;
	//	for (size_t i = 0; i < numEntities; i++)
	//	{
	//		block[i + 1];
	//	}
	//	//block[1] |= (1 << bitPosition);
	//	block[1] = registry->GetNumberOfComponentTypes();
	//}

	// Setters

	// Add an entity with given types
	Entity* EntityManager::AddEntity(std::vector<ComponentTypes> components)
	{
		if (components.size() != TYPE_COUNT)
			throw std::runtime_error("Size mismatch between components and ComponentTypes");

		std::vector<int> typeIndexList;
		ComponentTypes type;
		int index;

		for (int i = 0; i < TYPE_COUNT; i++)
		{
			type = static_cast<ComponentTypes>(i);
			typeIndexList.push_back(-1);
		}

		for (int i = 0; i < components.size(); i++)
		{
			switch (components[i])
			{
			case CAMERA:
				index = AddComponent(CAMERA);
				break;
			case NETWORK:
				index = AddComponent(NETWORK);
				break;
			case PHYSICS:
				index = AddComponent(PHYSICS);
				break;
			case RENDER:
				index = AddComponent(RENDER);
				break;
			default:
				throw std::runtime_error("Unknown component type");
			}
			typeIndexList[type] = index;
		}

		Entity entity = Entity(this, entities.size(), typeIndexList);
		entities.push_back(entity);

		for (int i = 0; i < TYPE_COUNT; i++)
		{
			if (typeIndexList[i] != -1)
			{
				entitiesWithType[i].push_back(entities.size() - 1);
			}
		}

		return &entities[entities.size() - 1];
	}

	// Clears the manager on disconnect
	void EntityManager::ClearManager()
	{
		entities.clear();

		ComponentTypes type;
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			type = static_cast<ComponentTypes>(i);
			(*componentMap[type]).clear();
			entitiesWithType[i].clear();
		}
	}

	// Private used in AddEntity - doesn't add to entity's
	// vector so mustn't be used except when making a new entity
	int EntityManager::AddComponent(ComponentTypes type)
	{
		int index;
		std::vector<std::unique_ptr<ComponentBase>>& vec = (*componentMap[type]);
		index = vec.size();
		switch (type)
		{
		case CAMERA:
			(*componentMap[CAMERA]).emplace_back(std::make_unique<CameraComponent>());
			break;
		case NETWORK:
			(*componentMap[NETWORK]).emplace_back(std::make_unique<NetworkComponent>());
			break;
		case PHYSICS:
			(*componentMap[PHYSICS]).emplace_back(std::make_unique<PhysicsComponent>());
			break;
		case RENDER:
			(*componentMap[RENDER]).emplace_back(std::make_unique<RenderComponent>());
			break;
		default:
			throw std::runtime_error("Unknown component type");
		}
		return index;
	}
}

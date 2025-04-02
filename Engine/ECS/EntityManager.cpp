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

		int noEntities = GetNumberOfEntities();
		size_t totalSize = noEntities + noEntities * entities[0].GetEntitySize();
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
		size_t totalSize = ComponentSizes[type] * (*componentMap[type]).size();
		std::vector<uint8_t> data(totalSize);
		size_t offset = 0;

		for (int i = 0; i < (*componentMap[type]).size(); i++)
		{
			(*componentMap[type])[i]->GetDataArray(data.data() + offset);

			offset += ComponentSizes[type];
		}
		return data;
	}

	// Format is
	// | # entities |
	// | # components | * # types
	// | vector of componentIndex | * # entities
	// | components of type | * # of component types
	// Get component data of all entities for all component type
	void EntityManager::GetAllData(uint8_t* block)
	{
		size_t offset = 0;
		ComponentTypes type;
		int numEntities = entities.size();

		block[offset++] = static_cast<uint8_t>(numEntities);

		for (int i = 0; i < TYPE_COUNT; i++)
		{
			type = static_cast<ComponentTypes>(i);
			block[offset++] = static_cast<uint8_t>(GetComponentTypeSize(type));
		}

		std::vector<int> ComponentIndexArray;
		for (int i = 0; i < numEntities; i++)
		{
			ComponentIndexArray = entities[i].GetComponentIndexArray();
			for (int j = 0; j < TYPE_COUNT; j++)
			{
				block[offset + i * TYPE_COUNT + j] = static_cast<uint8_t>(ComponentIndexArray[j]);
			}
		}

		offset += numEntities * TYPE_COUNT;

		std::vector<std::unique_ptr<ComponentBase>>* components;
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			type = static_cast<ComponentTypes>(i);
			components = GetComponentsOfType(type);
			for (int j = 0; j < components->size(); j++)
			{
				(*components)[i].get()->GetDataArray(block + offset);
			}
		}
	}

	// Setters

	// Set component data of all entities for all component type
	void EntityManager::SetAllData(uint8_t* block)
	{
		size_t offset = 0;
		int numEntities = static_cast<int>(block[offset++]);

		std::vector<int> componentCounts = std::vector<int>(TYPE_COUNT);
		std::vector<int> componentOffsets = std::vector<int>(TYPE_COUNT);

		for (int i = 0; i < TYPE_COUNT; i++)
		{
			componentCounts[i] = static_cast<int>(block[offset++]);
		}
		int componentSizeCount = 1 + TYPE_COUNT + numEntities * TYPE_COUNT;
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			componentSizeCount += componentCounts[i] * ComponentSizes[static_cast<ComponentTypes>(i)];
			componentOffsets[i] = componentSizeCount;
		}

		std::vector<int> componentIndexArray = std::vector<int>(TYPE_COUNT);
		std::vector<ComponentTypes> types;
		int typesSize;

		for (int i = 0; i < numEntities; i++)
		{
			types = std::vector<ComponentTypes>(TYPE_COUNT);
			typesSize = 0;
			for (int j = 0; j < TYPE_COUNT; j++)
			{
				componentIndexArray[j] = block[offset + i * TYPE_COUNT + j];
				if (componentIndexArray[j] != -1)
				{
					types.emplace_back(static_cast<ComponentTypes>(j));
					typesSize++;
				}
			}
			types.resize(typesSize);
			Entity* entity = AddEntity(types);
			ComponentBase* base;
			for (int j = 0; j < typesSize; j++)
			{
				base = GetComponentOfEntity(entity->GetEntityId(), types[j]);
				switch (types[j])
				{
				case CAMERA:
					reinterpret_cast<CameraComponent*>(base)->SetDataArray(block + componentOffsets[i] + componentIndexArray[types[j]]);
					break;
				case NETWORK:
					reinterpret_cast<NetworkComponent*>(base)->SetDataArray(block + componentOffsets[i] + componentIndexArray[types[j]]);
					break;
				case PHYSICS:
					reinterpret_cast<PhysicsComponent*>(base)->SetDataArray(block + componentOffsets[i] + componentIndexArray[types[j]]);
					break;
				case RENDER:
					reinterpret_cast<RenderComponent*>(base)->SetDataArray(block + componentOffsets[i] + componentIndexArray[types[j]]);
					break;
				default:
					throw std::runtime_error("Unknown component type");
				}
			}
		}

		int i = 0;

		/*std::vector<int> ComponentIndexArray;
		for (int i = 0; i < numEntities; i++)
		{
			ComponentIndexArray = entities[i].GetComponentIndexArray();
			for (int j = 1; j < TYPE_COUNT + 1; j++)
			{
				block[i * TYPE_COUNT + j] = static_cast<uint8_t>(ComponentIndexArray[j]);
			}
		}

		offset += numEntities * TYPE_COUNT;

		std::vector<std::unique_ptr<ComponentBase>>* components;
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			components = GetComponentsOfType(static_cast<ComponentTypes>(i));
			for (int j = 0; j < components->size(); j++)
			{
				(*components)[i].get()->GetDataArray(block + offset);
			}
		}*/
	}

	// Add an entity with given types
	Entity* EntityManager::AddEntity(std::vector<ComponentTypes> components)
	{
		std::vector<int> typeIndexList;
		int index;

		for (int i = 0; i < TYPE_COUNT; i++)
		{
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
			typeIndexList[components[i]] = index;
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

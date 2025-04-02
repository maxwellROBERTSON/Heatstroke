#include <algorithm>

#include "EntityManager.hpp"

namespace Engine
{
	// Constructor
	EntityManager::EntityManager()
	{
		// Check map size
		if (componentMap.size() != ComponentTypes::TYPE_COUNT)
		{
			throw std::runtime_error("Size mismatch between componentMap and ComponentTypes");
		}
		// Check if ComponentSizes is correct
		CameraComponent c;
		if (ComponentSizes[CAMERA] != c.StaticSize())
			throw std::runtime_error("Size mismatch between: ComponentSizes[CAMERA] and Camera size");
		NetworkComponent n;
		if (ComponentSizes[NETWORK] != n.StaticSize())
			throw std::runtime_error("Size mismatch between: ComponentSizes[NETWORK] and Network size");
		PhysicsComponent p;
		if (ComponentSizes[PHYSICS] != p.StaticSize())
			throw std::runtime_error("Size mismatch between: ComponentSizes[PHYSICS] and Physics size");
		RenderComponent r;
		if (ComponentSizes[RENDER] != r.StaticSize())
			throw std::runtime_error("Size mismatch between: ComponentSizes[RENDER] and Render size");
	};

	// Getters

	// Format is
	// | # entities |
	// | entity_data | * # entites
	// | # components of type | * # types
	// | vector of componentIndex | * # entities
	// | components of type | * # of component types
	// Get total data size
	size_t EntityManager::GetTotalDataSize()
	{
		if (entities.size() == 0)
			return 0;

		int numEntities = GetNumberOfEntities();
		int entitySize = entities[0].GetEntitySize();
		size_t totalSize = 1 + entities[0].GetEntitySize() * numEntities + TYPE_COUNT + TYPE_COUNT * numEntities;
		ComponentTypes type;
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			type = static_cast<ComponentTypes>(i);
			totalSize += (*componentMap[type]).size() * ComponentSizes[type];
		}
		return totalSize;
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
	// | entity_data | * # entites
	// | # components of type | * # types
	// | vector of componentIndex | * # entities
	// | components of type | * # of component types
	// Get component data of all entities for all component type
	void EntityManager::GetAllData(uint8_t* block)
	{
		size_t offset = 0;
		ComponentTypes type;
		int numEntities = entities.size();

		// | # entities |
		block[offset++] = static_cast<uint8_t>(numEntities);

		// | entity_data | * # entites
		int sizeOfEntity = entities[0].GetEntitySize();
		for (int i = 0; i < numEntities; i++)
		{
			entities[i].GetDataArray(block + offset);
			offset += sizeOfEntity;
		}

		// | # components of type | * # types
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			type = static_cast<ComponentTypes>(i);
			block[offset++] = static_cast<uint8_t>(GetComponentTypeSize(type));
		}

		// | vector of componentIndex | * # entities
		std::vector<int> ComponentIndexArray;
		for (int i = 0; i < numEntities; i++)
		{
			ComponentIndexArray = entities[i].GetComponentIndexArray();
			for (int j = 0; j < TYPE_COUNT; j++)
			{
				uint8_t temp = static_cast<uint8_t>(ComponentIndexArray[j]);
				block[offset + i * TYPE_COUNT + j] = static_cast<uint8_t>(ComponentIndexArray[j]);
			}
		}

		offset += numEntities * TYPE_COUNT;

		// | components of type | * # of component types
		std::vector<std::unique_ptr<ComponentBase>>* components;
		size_t sizeOfComponent;
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			type = static_cast<ComponentTypes>(i);
			components = GetComponentsOfType(type);
			sizeOfComponent = ComponentSizes[type];
			std::cout << "Start of component " << i << " is " << reinterpret_cast<uintptr_t>(block + offset) << std::endl;
			for (int j = 0; j < components->size(); j++)
			{
				(*components)[j].get()->GetDataArray(block + offset);
				offset += sizeOfComponent;
			}
			std::cout << "End of component " << i << " is " << reinterpret_cast<uintptr_t>(block + offset) << std::endl;
		}
	}

	// Setters

	// Format is
	// | # entities |
	// | entity_data | * # entites
	// | # components of type | * # types
	// | vector of componentIndex | * # entities
	// | components of type | * # of component types
	// Set component data of all entities for all component type
	void EntityManager::SetAllData(uint8_t* block)
	{
		size_t offset = 0;
		uint8_t* f;

		// | # entities |
		int numEntities = static_cast<int>(block[offset++]);
		f = block + offset;

		// | entity_data | * # entites
		for (int i = 0; i < numEntities; i++)
		{
			Entity entity = Entity(this);
			entity.SetDataArray(block + offset);
			entities.push_back(entity);
			offset += entities[0].GetEntitySize();
			f = block + offset;
		}

		// | # components of type | * # types
		std::vector<int> componentCounts = std::vector<int>(TYPE_COUNT);
		std::vector<int> componentOffsets = std::vector<int>(TYPE_COUNT);
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			componentCounts[i] = static_cast<int>(block[offset++]);
		}
		int componentsSizeCount = 0;
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			componentOffsets[i] = offset + TYPE_COUNT * numEntities + componentsSizeCount;
			componentsSizeCount += componentCounts[i] * ComponentSizes[static_cast<ComponentTypes>(i)];
			std::cout << "Assumed start of component " << i << " is " << reinterpret_cast<uintptr_t>(block + componentOffsets[i]) << std::endl;
		}

		f = block + offset;

		std::vector<int> componentIndexArray = std::vector<int>(TYPE_COUNT);
		std::vector<ComponentTypes> types;
		int typesSize;
		Entity* entity;

		for (int i = 0; i < numEntities; i++)
		{
			// | vector of componentIndex | * # entities
			types = std::vector<ComponentTypes>(0);
			typesSize = 0;
			for (int j = 0; j < TYPE_COUNT; j++)
			{
				componentIndexArray[j] = block[offset + i * TYPE_COUNT + j];
				if (componentIndexArray[j] != 255)
				{
					types.emplace_back(static_cast<ComponentTypes>(j));
					typesSize++;
				}
			}
			types.resize(typesSize);
			entity = &entities[i];
			AddEntity(&entities[i], types);
			std::cout << "ADDED entity " << i << std::endl;

			// | components of type | * # of component types
			ComponentBase* base;
			for (int j = 0; j < typesSize; j++)
			{
				base = GetComponentOfEntity(entity->GetEntityId(), types[j]);
				switch (types[j])
				{
				case CAMERA:
					std::cout << "Setting the " << componentIndexArray[types[j]] << " of component of type " << types[j] << " at ";
					std::cout << reinterpret_cast<uintptr_t>(block + componentOffsets[types[j]] + componentIndexArray[types[j]] * ComponentSizes[CAMERA]) << std::endl;
					f = block + componentOffsets[types[j]] + componentIndexArray[types[j]] * ComponentSizes[CAMERA];
					reinterpret_cast<CameraComponent*>(base)->SetDataArray(block + componentOffsets[types[j]] + componentIndexArray[types[j]] * ComponentSizes[CAMERA]);
					break;
				case NETWORK:
					std::cout << "Setting the " << componentIndexArray[types[j]] << " of component of type " << types[j] << " at ";
					std::cout << reinterpret_cast<uintptr_t>(block + componentOffsets[types[j]] + componentIndexArray[types[j]] * ComponentSizes[NETWORK]) << std::endl;
					f = block + componentOffsets[types[j]] + componentIndexArray[types[j]] * ComponentSizes[NETWORK];
					reinterpret_cast<NetworkComponent*>(base)->SetDataArray(block + componentOffsets[types[j]] + componentIndexArray[types[j]] * ComponentSizes[NETWORK]);
					break;
				case PHYSICS:
					std::cout << "Setting the " << componentIndexArray[types[j]] << " of component of type " << types[j] << " at ";
					std::cout << reinterpret_cast<uintptr_t>(block + componentOffsets[types[j]] + componentIndexArray[types[j]] * ComponentSizes[PHYSICS]) << std::endl;
					f = block + componentOffsets[types[j]] + componentIndexArray[types[j]] * ComponentSizes[PHYSICS];
					reinterpret_cast<PhysicsComponent*>(base)->SetDataArray(block + componentOffsets[types[j]] + componentIndexArray[types[j]] * ComponentSizes[PHYSICS]);
					break;
				case RENDER:
					std::cout << "Setting the " << componentIndexArray[types[j]] << " of component of type " << types[j] << " at ";
					std::cout << reinterpret_cast<uintptr_t>(block + componentOffsets[types[j]] + componentIndexArray[types[j]] * ComponentSizes[RENDER]) << std::endl;
					f = block + componentOffsets[types[j]] + componentIndexArray[types[j]] * ComponentSizes[RENDER];
					reinterpret_cast<RenderComponent*>(base)->SetDataArray(block + componentOffsets[types[j]] + componentIndexArray[types[j]] * ComponentSizes[RENDER]);
					break;
				default:
					throw std::runtime_error("Unknown component type");
				}
				std::cout << "ADDED component " << types[j] << " to entity " << entity->GetEntityId() << std::endl;
			}
		}

		int i = 0;

		// PHYSICS COMPONENT MISALIGNMENT

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

	// Add an existing entity to the manager
	void EntityManager::AddEntity(Entity* entity, std::vector<ComponentTypes> components)
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

		entity->SetComponentIndexArray(typeIndexList);

		for (int i = 0; i < TYPE_COUNT; i++)
		{
			if (typeIndexList[i] != -1)
			{
				entitiesWithType[i].push_back(entity->GetEntityId());
			}
		}
	}

	// Make a new entity with given types
	Entity* EntityManager::MakeNewEntity(std::vector<ComponentTypes> components)
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

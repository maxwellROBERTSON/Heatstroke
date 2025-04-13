#include <algorithm>

#include "EntityManager.hpp"
#include "Components/Component.hpp"
#include "Components/CameraComponent.hpp"
#include "Components/NetworkComponent.hpp"
#include "Components/PhysicsComponent.hpp"
#include "Components/RenderComponent.hpp"

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
		int entitySize = entities[0].get()->GetEntitySize();
		size_t totalSize = 1 + entities[0].get()->GetEntitySize() * numEntities + TYPE_COUNT + TYPE_COUNT * numEntities;
		ComponentTypes type;
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			type = static_cast<ComponentTypes>(i);
			totalSize += (*componentMap[type]).size() * ComponentSizes[type];
		}
		return totalSize;
	}

	// Format is
	// | # changed entities |
	// | entity ID's | * # changed entities
	// | # changed entity / components | * # types + 1
	// | num bytes = (num componentTypes + 2 / 8) + 1 | * # changed entities
	// above bits is form: MSB = e, each less bit corresponds to a componentType
	// if that componentType for this entity is changed, bit is on
	// | entity_data | * # changed entites
	// | components of type | * # of changed components of types
	// Get total size of all the changed data
	size_t EntityManager::GetTotalChangedDataSize()
	{
		size_t baseSize = 1 + changedEntitiesAndComponents.size() * 3;
		size_t totalSize = baseSize;

		int entitySize = entities[0].get()->GetEntitySize();
		for (int i = 0; i < changedEntitiesAndComponents.size(); i++)
		{
			if (changedEntitiesAndComponents[i].second[0] != -1)
			{
				totalSize += entitySize;
			}
			for (int j = 1; j < TYPE_COUNT + 1; j++)
			{
				if (changedEntitiesAndComponents[i].second[j] != -1)
				{
					totalSize += ComponentSizes[static_cast<ComponentTypes>(j - 1)];
				}
			}
		}

		if (totalSize == baseSize)
		{
			return 0;
		}
		else
		{
			return totalSize;
		}
	}

	// Get a pointer to the component of an entity
	ComponentBase* EntityManager::GetComponentOfEntity(int entityId, ComponentTypes type)
	{
		int index = entities[entityId].get()->GetComponent(type);
		if (index == -1)
		{
			return nullptr;
		}
		else
		{
			return (*componentMap[type])[index].get();
		}
	}

	// Get a pointer to all the components of a type
	std::vector<std::unique_ptr<ComponentBase>>* EntityManager::GetComponentsOfType(ComponentTypes type) {
		auto it = componentMap.find(type);
		if (it != componentMap.end() && !it->second->empty())
		{
			return it->second;
		}

		// Return nullptr if not found or vector is empty
		return nullptr;
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
		int sizeOfEntity = entities[0].get()->GetEntitySize();
		for (int i = 0; i < numEntities; i++)
		{
			entities[i].get()->GetDataArray(block + offset);
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
			ComponentIndexArray = entities[i].get()->GetComponentIndexArray();
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
			for (int j = 0; j < components->size(); j++)
			{
				(*components)[j].get()->GetDataArray(block + offset);
				offset += sizeOfComponent;
			}
		}
	}

	// Format is
	// | # changed entities |
	// | entity ID's | * # changed entities
	// | # changed entity / components | * # types + 1
	// | num bytes = (num componentTypes + 2 / 8) + 1 | * # changed entities
	// above bits is form: MSB = e, each less bit corresponds to a componentType
	// if that componentType for this entity is changed, bit is on
	// | entity_data | * # changed entites
	// | components of type | * # of changed components of types
	// Get all changed entity and component data
	void EntityManager::GetAllChangedData(uint8_t* block)
	{
		size_t offset = 0;
		ComponentTypes type;
		int numChangedEntities = changedEntitiesAndComponents.size();

		// | # changed entities |
		block[offset++] = static_cast<uint8_t>(numChangedEntities);

		// | entity ID's | * # changed entities
		for (auto& [entity, flags] : changedEntitiesAndComponents)
		{
			block[offset++] = static_cast<uint8_t>(entity);
		}

		// | # changed entity / components | * # types + 1
		for (int i = 0; i < TYPE_COUNT + 1; i++)
		{
			int count = 0;
			for (auto& [entity, flags] : changedEntitiesAndComponents)
			{
				if (flags[i] != -1)
					count++;
			}
			block[offset++] = static_cast<uint8_t>(count);
		}

		// | num bytes = (num componentTypes + 2 / 8) + 1 | * # changed entities
		int numBytesPer = (TYPE_COUNT + 2) / 8 + 1;

		for (auto& [entity, flags] : changedEntitiesAndComponents)
		{
			for (int i = 0; i < numBytesPer; i++)
			{
				uint8_t byte = 0;
				for (int j = 0; j < 8 && (i * 8 + j) < flags.size(); j++)
				{
					if (flags[i * 8 + j] != -1)
					{
						byte |= (1 << (7 - j));
					}
				}
				block[offset++] = byte;
			}
		}

		// | entity_data | * # changed entites
		int sizeOfEntity = entities[0].get()->GetEntitySize();
		for (auto& [entity, flags] : changedEntitiesAndComponents)
		{
			if (flags[0] == 1)
			{
				GetEntity(entity)->GetDataArray(block + offset);
				offset += sizeOfEntity;
			}
		}

		// | components of type | * # of changed components of types
		std::vector<std::unique_ptr<ComponentBase>>* components;
		size_t sizeOfComponent;
		for (int i = 1; i < TYPE_COUNT + 1; i++)
		{
			type = static_cast<ComponentTypes>(i - 1);
			components = GetComponentsOfType(type);
			sizeOfComponent = ComponentSizes[type];
			for (int j = 0; j < changedEntitiesAndComponents.size(); j++)
			{
				int index = changedEntitiesAndComponents[j].second[i];
				if (index != -1)
				{
					(*components)[index].get()->GetDataArray(block + offset);
					offset += sizeOfComponent;
				}
			}
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
			entities.push_back(std::make_unique<Entity>(this));
			Entity* entity = entities.back().get();
			entity->SetDataArray(block + offset);
			offset += entities[0].get()->GetEntitySize();
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
			entity = entities[i].get();
			AddEntity(entity, types);
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
	}

	// Format is
	// | # changed entities |
	// | entity ID's | * # changed entities
	// | # changed entity / components | * # types + 1
	// | num bytes = (num componentTypes + 2 / 8) + 1 | * # changed entities
	// above bits is form: MSB = e, each less bit corresponds to a componentType
	// if that componentType for this entity is chaned, bit is on
	// | entity_data | * # changed entites
	// | components of type | * # of changed components of types
	// Set all changed entity and component data
	void EntityManager::SetAllChangedData(uint8_t* block)
	{
		size_t offset = 0;
		uint8_t* f;

		// | # changed entities |
		int numChangedEntities = static_cast<int>(block[offset++]);
		f = block + offset;

		// | entity ID's | * # changed entities
		std::vector<int> entityIdEntityComponentIndex = std::vector<int>(numChangedEntities);
		for (int i = 0; i < numChangedEntities; i++)
		{
			entityIdEntityComponentIndex[i] = static_cast<int>(block[offset++]);
		}

		// | # changed entity / components | * # types + 1
		std::vector<int> componentEntityCounts = std::vector<int>(TYPE_COUNT + 1);
		for (int i = 0; i < TYPE_COUNT + 1; i++)
		{
			componentEntityCounts[i] = static_cast<int>(block[offset++]);
		}
		std::vector<int> entityComponentOffsets = std::vector<int>(TYPE_COUNT + 1);
		Entity entity(this);
		int numBytesPer = (TYPE_COUNT + 2) / 8 + 1;
		int dataOffset = offset + numBytesPer * numChangedEntities;
		entityComponentOffsets[0] = dataOffset;
		size_t entitySize = entity.GetEntitySize();
		dataOffset += entitySize * componentEntityCounts[0];
		for (int i = 1; i < TYPE_COUNT + 1; i++)
		{
			entityComponentOffsets[i] = dataOffset;
			dataOffset += ComponentSizes[static_cast<ComponentTypes>(i - 1)] * componentEntityCounts[i];
		}

		// | num bytes = (num componentTypes + 2 / 8) + 1 | * # changed entities
		Entity* entityPtr;
		ComponentBase* base;
		ComponentTypes type;
		for (int i = 0; i < numChangedEntities; i++)
		{
			entityPtr = GetEntity(entityIdEntityComponentIndex[i]);
			for (int j = 0; j < numBytesPer; j++)
			{
				uint8_t byte = block[offset++];

				// Now, for each bit in the byte, you can check its value
				for (int k = 0; k < 8 && (j * 8 + k) < TYPE_COUNT + 1; k++)
				{
					if (byte & (1 << (7 - k))) // If the corresponding bit is set
					{
						if ((j * 8 + k) == 0)
						{
							entityPtr->SetDataArray(block + entityComponentOffsets[0]);
							entityComponentOffsets[0] += entitySize;
							continue;
						}

						type = static_cast<ComponentTypes>((j * 8 + k - 1));
						base = GetComponentOfEntity(entityPtr->GetEntityId(), type);

						switch (type)
						{
						case CAMERA:
							reinterpret_cast<CameraComponent*>(base)->SetDataArray(block + entityComponentOffsets[(j * 8 + k)]);
							entityComponentOffsets[(j * 8 + k)] += ComponentSizes[CAMERA];
							break;
						case NETWORK:
							reinterpret_cast<NetworkComponent*>(base)->SetDataArray(block + entityComponentOffsets[(j * 8 + k)]);
							entityComponentOffsets[(j * 8 + k)] += ComponentSizes[NETWORK];
							break;
						case PHYSICS:
							reinterpret_cast<PhysicsComponent*>(base)->SetDataArray(block + entityComponentOffsets[(j * 8 + k)]);
							entityComponentOffsets[(j * 8 + k)] += ComponentSizes[PHYSICS];
							break;
						case RENDER:
							reinterpret_cast<RenderComponent*>(base)->SetDataArray(block + entityComponentOffsets[(j * 8 + k)]);
							entityComponentOffsets[(j * 8 + k)] += ComponentSizes[RENDER];
							break;
						default:
							throw std::runtime_error("Unknown component type");
						}
						std::cout << "Edited component " << type << " to entity " << entity.GetEntityId() << std::endl;
					}
				}
			}
		}
	}

	// Reset changed entities and components
	void EntityManager::ResetChanged()
	{
		Entity* entity;
		std::vector<int> indexArray;
		ComponentBase* base;
		ComponentTypes type;
		entities;
		for (int i = 0; i < changedEntitiesAndComponents.size(); i++)
		{
			entity = GetEntity(changedEntitiesAndComponents[i].first);
			indexArray = entity->GetComponentIndexArray();
			if (changedEntitiesAndComponents[i].second[0] != -1)
				entity->ToggleHasChanged();
			for (int j = 1; j < TYPE_COUNT + 1; j++)
			{
				if (indexArray[j - 1] == -1 || changedEntitiesAndComponents[i].second[j] == -1)
					continue;
				type = static_cast<ComponentTypes>(j - 1);
				base = GetComponentOfEntity(changedEntitiesAndComponents[i].first, type);
				switch (type)
				{
				case CAMERA:
					reinterpret_cast<CameraComponent*>(base)->ToggleHasChanged();
					break;
				case NETWORK:
					reinterpret_cast<NetworkComponent*>(base)->ToggleHasChanged();
					break;
				case PHYSICS:
					reinterpret_cast<PhysicsComponent*>(base)->ToggleHasChanged();
					break;
				case RENDER:
					reinterpret_cast<RenderComponent*>(base)->ToggleHasChanged();
					break;
				default:
					throw std::runtime_error("Unknown component type");
				}
			}
		}
		changedEntitiesAndComponents = std::vector<std::pair<int, std::vector<int>>>(0);
	}

	// Add changed entity
	void EntityManager::AddChangedEntity(Entity* entity)
	{
		std::cout << "Entity " << entity->GetEntityId() << " has changed." << std::endl;
		int id = entity->GetEntityId();
		for (auto& pair : changedEntitiesAndComponents)
		{
			if (pair.first == id)
			{
				return;
			}
		}

		std::vector<int> componentFlags(TYPE_COUNT + 1, -1);
		componentFlags[0] = 1;
		changedEntitiesAndComponents.emplace_back(id, componentFlags);
	}

	// Add changed component
	void EntityManager::AddChangedComponent(ComponentTypes type, Entity* entity)
	{
		std::cout << "Entity " << entity->GetEntityId() << "'s " << type << " component has changed." << std::endl;
		int id = entity->GetEntityId();
		for (auto& pair : changedEntitiesAndComponents)
		{
			if (pair.first == id)
			{
				pair.second[static_cast<int>(type) + 1] = entity->GetComponent(type);
				return;
			}
		}

		// Not found, add new pair and immediately set the flag
		std::vector<int> componentFlags(TYPE_COUNT + 1, -1);
		componentFlags[static_cast<int>(type) + 1] = entity->GetComponent(type);

		changedEntitiesAndComponents.emplace_back(id, std::move(componentFlags));
	}

	// Set next network component unassigned to a client
	void EntityManager::AssignNextClient(uint64_t clientId)
	{
		this;
		NetworkComponent* comp = reinterpret_cast<NetworkComponent*>((*componentMap[NETWORK])[nextNetworkComponent].get());
		comp->SetClientId(clientId);
	}

	// Add an existing entity to the manager
	// ComponentIndexArray overwritten so only use if components not yet initialised
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
				index = AddComponent(CAMERA, entity);
				break;
			case NETWORK:
				index = AddComponent(NETWORK, entity);
				break;
			case PHYSICS:
				index = AddComponent(PHYSICS, entity);
				break;
			case RENDER:
				index = AddComponent(RENDER, entity);
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

		entities.push_back(std::make_unique<Entity>(this));
		Entity* entity = entities.back().get();

		for (int i = 0; i < components.size(); i++)
		{
			switch (components[i])
			{
			case CAMERA:
				index = AddComponent(CAMERA, entity);
				break;
			case NETWORK:
				index = AddComponent(NETWORK, entity);
				break;
			case PHYSICS:
				index = AddComponent(PHYSICS, entity);
				break;
			case RENDER:
				index = AddComponent(RENDER, entity);
				break;
			default:
				throw std::runtime_error("Unknown component type");
			}
			typeIndexList[components[i]] = index;
		}

		entity->SetComponentIndexArray(typeIndexList);
		entity->SetEntityId(entities.size() - 1);

		for (int i = 0; i < TYPE_COUNT; i++)
		{
			if (typeIndexList[i] != -1)
			{
				entitiesWithType[i].push_back(entities.size() - 1);
			}
		}

		return entity;
	}

	// Clears the manager on disconnect
	void EntityManager::ClearManager()
	{
		entities.clear();

		changedEntitiesAndComponents.clear();

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
	int EntityManager::AddComponent(ComponentTypes type, Entity* entity)
	{
		if (type == NETWORK)
			int ls = 0;
		int index;
		std::vector<std::unique_ptr<ComponentBase>>& vec = (*componentMap[type]);
		index = vec.size();
		switch (type)
		{
		case CAMERA:
			(*componentMap[CAMERA]).emplace_back(std::make_unique<CameraComponent>(this, entity));
			//std::cout << "Camera for " << entity << std::endl;
			break;
		case NETWORK:
			if (nextNetworkComponent == -1)
				nextNetworkComponent = 0;
			(*componentMap[NETWORK]).emplace_back(std::make_unique<NetworkComponent>(this, entity));
			//std::cout << "Network for " << entity << std::endl;
			break;
		case PHYSICS:
			(*componentMap[PHYSICS]).emplace_back(std::make_unique<PhysicsComponent>(this, entity));
			//std::cout << "Physics for " << entity << std::endl;
			break;
		case RENDER:
			(*componentMap[RENDER]).emplace_back(std::make_unique<RenderComponent>(this, entity));
			//std::cout << "Render for " << entity << std::endl;
			break;
		default:
			throw std::runtime_error("Unknown component type");
		}
		return index;
	}
}

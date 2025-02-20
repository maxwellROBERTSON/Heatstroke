#include <iostream>

#include "Entity.hpp"

// Public Methods

Entity::Entity(EntityManager* entityManager, int entityId, bool componentBitmask[], int componentIndex[]) : entityManager(entityManager), entityId(entityId)
{
	// Add all components to the entity
	for (int i = 0; i < sizeof(ComponentType); i++)
	{
		if (componentBitmask[i])
		{
			componentListId.push_back(std::make_pair(static_cast<ComponentType>(i), componentIndex[i]));
		}
		else
		{
			componentListId.push_back(std::make_pair(static_cast<ComponentType>(i), -1));
		}
	}
}

int Entity::HasComponent(ComponentType type)
{
	// Check ComponentType exists in entity's list
	for (int i = 0; i < componentListId.size(); i++)
	{
		if (componentListId[i].first == type)
		{
			return i;
		}
	}
	return -1;
}

//void Entity::AddComponent(ComponentType type)
//{
//	// Check ComponentType does not already exist in entity's list
//	for (int i = 0; i < componentListId.size(); i++)
//	{
//		if (componentListId[i].first == type)
//		{
//			std::cout << "Component of type " << static_cast<int>(type) << " already exists in entity with id " << entityId << std::endl;
//			return;
//		}
//	}
//
//	// Add to ComponentType list in EntityManager
//	entityManager->AddComponent(type);
//
//	//componentListId.push_back(std::make_pair(type, componentListId.size() + 1));
//}

int Entity::GetNumberOfComponents()
{
	return componentListId.size();
}

void Entity::Update(float deltaTime)
{
	// Update entity
}

// Private Methods

void Entity::LoadEntity()
{
	//tinygltf::Model tinygltfmodel = Engine::loadFromFile(file_location);
}
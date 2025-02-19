#include <iostream>

#include "Entity.hpp"

// Public Methods

Entity::Entity(EntityManager* entityManager, int entityId) : entityManager(entityManager), entityId(entityId)
{
	entityManager->AddEntity(this);
}

void Entity::AddComponent(ComponentType type)
{
	// Check ComponentType does not already exist in entity's list
	for (int i = 0; i < componentListId.size(); i++)
	{
		if (componentListId[i].first == type)
		{
			std::cout << "Component of type " << static_cast<int>(type) << " already exists in entity with id " << entityId << std::endl;
			return;
		}
	}

	// Add to ComponentType list in EntityManager
	entityManager->AddComponent(type);

	//componentListId.push_back(std::make_pair(type, componentListId.size() + 1));
}

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
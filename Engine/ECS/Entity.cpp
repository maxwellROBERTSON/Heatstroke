#include <iostream>

#include "Entity.hpp"

// Public Methods

Entity::Entity(EntityManager* entityManager, int entityId, std::vector<int> typeIndexList)
	: entityManager(entityManager), entityId(entityId)
{
	// Add all components to the entity
	for (int i = 0; i < typeIndexList.size(); i++)
	{
		componentListId.push_back(typeIndexList[i]);
	}
}

void Entity::GetData(uint8_t* block)
{
	// Matrix
	uint8_t* bytePtr = reinterpret_cast<uint8_t*>(&modelMatrix);
	for (size_t i = 0; i < sizeof(glm::mat4); ++i) {
		block[i] = bytePtr[i];
	}
	block = block + sizeof(glm::mat4)
	// Component List

}

void Entity::SetData(uint8_t* block)
{

}

// Private Methods

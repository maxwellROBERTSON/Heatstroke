#include <iostream>

#include "Entity.hpp"
#include "EntityManager.hpp"

namespace Engine
{
	class EntityManager;

	// Constructor
	Entity::Entity(EntityManager* entityManager, int entityId, std::vector<int> indexList)
		: entityManager(entityManager), entityId(entityId)
	{
		// Add index list to the entity
		componentTypeIndexList = indexList;
	}

	// Getters

	// Get the data for a given entity
	void Entity::GetData(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(data + offset, &entityId, sizeof(entityId));
		offset += sizeof(entityId);
		std::memcpy(data + offset, &modelMatrix, sizeof(modelMatrix));
		offset += sizeof(modelMatrix);

		// If the entity has a component type add its data to the vector
		ComponentBase* component;
		for (int i = 0; i < TYPE_COUNT; i++)
		{
			if (componentTypeIndexList[i] != -1)
			{
				component = entityManager->GetComponentOfEntity(entityId, static_cast<ComponentTypes>(i));
				component->GetDataArray(data);
			}
		}

		//std::vector<uint8_t> data;
		//size_t numberOfComponents = componentListId.size();
		//size_t numberOfBytes = (numberOfComponents - 1) / 8 + 1;
		//uint8_t byte = 0;
		//data.insert(data.end(), numberOfBytes, byte);
		//for (size_t i = numberOfBytes; i > 0; i--)
		//{
		//	//data[i]
		//	for (size_t j = 0; j < componentListId.size(); j++)
		//	{
		//		if (componentListId[j] != -1)
		//		{
		//			entityManager->GetEntityComponent<entityManager->registry->GetTypeIndex(j)>(entityId);
		//			data[i] = entityManager->componentList[j].first[componentListId[j]];
		//		}
		//	}
		//}
		// 
		// 
		//// Matrix
		//uint8_t* bytePtr = reinterpret_cast<uint8_t*>(&modelMatrix);
		//for (size_t i = 0; i < sizeof(glm::mat4); ++i) {
		//	block[i] = bytePtr[i];
		//}
		//block = block + sizeof(glm::mat4);
		//// Component List

	}

	// Setters

	// Set the data for a given entity
	void Entity::SetData(uint8_t* block)
	{

	}

	glm::mat4 Entity::GetModelMatrix() {
		if (this->dirty) {
			this->modelMatrix = glm::translate(glm::mat4(1.0f), this->position) * this->rotation * glm::scale(glm::mat4(1.0f), this->scale);
			this->dirty = false;
		}
		return this->modelMatrix;
	}

	void Entity::SetPosition(float x, float y, float z) {
		this->position = glm::vec3(x, y, z);
		this->dirty = true;
	}

	void Entity::SetPosition(glm::vec3 position) {
		this->position = position;
		this->dirty = true;
	}

	void Entity::SetRotation(float angInDeg, glm::vec3 axis) {
		this->rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angInDeg), axis);
		this->dirty = true;
	}

	void Entity::SetRotation(glm::quat rotation) {
		this->rotation = glm::mat4(rotation);
		this->dirty = true;
	}

	void Entity::SetScale(float xScale, float yScale, float zScale) {
		this->scale = glm::vec3(xScale, yScale, zScale);
		this->dirty = true;
	}

	void Entity::SetScale(float overallScale) {
		this->scale = glm::vec3(overallScale);
		this->dirty = true;
	}

}
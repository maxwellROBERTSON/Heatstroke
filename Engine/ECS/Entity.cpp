#include <iostream>

#include "Entity.hpp"
#include "EntityManager.hpp"

namespace Engine
{
	class EntityManager;

	// Constructors

	Entity::Entity(EntityManager* entityManager)
		: entityManager(entityManager), entityId(-1)
	{
	}

	Entity::Entity(EntityManager* entityManager, int entityId, std::vector<int> indexList)
		: entityManager(entityManager), entityId(entityId)
	{
		// Add index list to the entity
		componentTypeIndexList = indexList;
	}

	// Getters

	// Get the data for a given entity
	void Entity::GetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(data + offset, &entityId, sizeof(entityId));
		offset += sizeof(entityId);
		std::memcpy(data + offset, &GetModelMatrix(), sizeof(GetModelMatrix()));
	}

	// Get model matrix
	glm::mat4 Entity::GetModelMatrix()
	{
		if (this->dirty) {
			this->modelMatrix = glm::translate(glm::mat4(1.0f), this->position) * this->rotation * glm::scale(glm::mat4(1.0f), this->scale);
			this->dirty = false;
		}
		return this->modelMatrix;
	}

	// Setters

	// Set the data for a given entity
	void Entity::SetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(&entityId, data + offset, sizeof(entityId));
		offset += sizeof(entityId);
		std::memcpy(&modelMatrix, data + offset, sizeof(modelMatrix));
	}

	// Position Setters
	void Entity::SetPosition(float x, float y, float z)
	{
		this->position = glm::vec3(x, y, z);
		this->dirty = true;
		SetEntityHasChanged();
	}
	void Entity::SetPosition(glm::vec3 position)
	{
		this->position = position;
		this->dirty = true;
		SetEntityHasChanged();
	}

	// Rotation setters
	void Entity::SetRotation(float angInDeg, glm::vec3 axis)
	{
		this->rotation = glm::rotate(glm::mat4(1.0f), glm::radians(angInDeg), axis);
		this->dirty = true;
		SetEntityHasChanged();
	}
	void Entity::SetRotation(glm::quat rotation)
	{
		this->rotation = glm::mat4(rotation);
		this->dirty = true;
		SetEntityHasChanged();
	}

	// Scale setters
	void Entity::SetScale(float xScale, float yScale, float zScale)
	{
		this->scale = glm::vec3(xScale, yScale, zScale);
		this->dirty = true;
		SetEntityHasChanged();
	}
	void Entity::SetScale(float overallScale)
	{
		this->scale = glm::vec3(overallScale);
		this->dirty = true;
		SetEntityHasChanged();
	}

	// Set component has changed in entity manager
	void Entity::SetEntityHasChanged()
	{
		if (!hasChanged)
		{
			entityManager->AddChangedEntity(this);
			hasChanged = true;
		}
	}
}

#include <iostream>

#include "Entity.hpp"
#include "EntityManager.hpp"

#include <glm/gtx/string_cast.hpp>

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
		glm::mat4 mat = GetModelMatrix();
		std::memcpy(data + offset, &mat, sizeof(GetModelMatrix()));
	}

	// Get model matrix
	glm::mat4 Entity::GetModelMatrix()
	{
		if (this->dirty)
		{
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
		
		if (std::memcmp(&entityId, data + offset, sizeof(entityId)) != 0)
		{
			std::memcpy(&entityId, data + offset, sizeof(entityId));
			SetEntityHasChanged();
		}
		offset += sizeof(entityId);
		if (std::memcmp(&modelMatrix, data + offset, sizeof(modelMatrix)) != 0)
		{
			std::memcpy(&modelMatrix, data + offset, sizeof(modelMatrix));
			SetEntityHasChanged();
		}
	}

	// Position Setters
	void Entity::SetPosition(float x, float y, float z)
	{
		glm::vec3 vec = glm::vec3(x, y, z);
		if (this->position != vec)
		{
			this->position = vec;
			this->dirty = true;
			SetEntityHasChanged();
		}
	}
	
	void Entity::SetPosition(glm::vec3 position)
	{
		if (this->position != position)
		{
			this->position = position;
			this->dirty = true;
			SetEntityHasChanged();
		}
	}

	void Entity::SetRotation(float angInDeg, glm::vec3 axis) 
	{
		glm::mat4 rotate = glm::rotate(glm::mat4(1.0f), glm::radians(angInDeg), axis);
		if (this->rotation != rotate)
		{
			this->rotation = rotate;
			this->dirty = true;
			SetEntityHasChanged();
		}
	}

	// from input actions
	void Entity::SetRotation(glm::mat4 aRotation)
	{
		this->rotation = aRotation;
		this->dirty = true;
		SetEntityHasChanged();
	}

	void Entity::SetRotation(glm::quat rotation)
	{
		glm::mat4 rotate = glm::mat4(rotation);
		if (this->rotation != rotate)
		{
			this->rotation = rotate;
			this->dirty = true;
			SetEntityHasChanged();
		}
	}

	// Scale setters
	void Entity::SetScale(float xScale, float yScale, float zScale)
	{
		glm::vec3 scale = glm::vec3(xScale, yScale, zScale);
		if (this->scale != scale)
		{
			this->scale = scale;
			this->dirty = true;
			SetEntityHasChanged();
		}
	}

	void Entity::SetScale(float overallScale)
	{
		glm::vec3 scale = glm::vec3(overallScale);
		if (this->scale != scale)
		{
			this->scale = scale;
			this->dirty = true;
			SetEntityHasChanged();
		}
	}

	// Set model matrix with check for if changed
	void Entity::SetModelMatrix(glm::mat4 aModelMatrix)
	{
		if (aModelMatrix != modelMatrix)
		{
			modelMatrix = aModelMatrix;
			SetEntityHasChanged();
		}
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

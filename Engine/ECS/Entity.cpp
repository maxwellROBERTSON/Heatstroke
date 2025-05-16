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
		if (indexList.size() != TYPE_COUNT)
			throw("Index list for MakeNewEntity not equal to TYPE_COUNT");
		componentTypeIndexList = indexList;
	}

	// Getters

	// Get the data for a given entity
	void Entity::GetDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(data + offset, &entityId, sizeof(entityId));
		offset += sizeof(entityId);
		std::memcpy(data + offset, &position, sizeof(position));
		offset += sizeof(position);
		std::memcpy(data + offset, &rotation, sizeof(rotation));
	}

	// Get the initial data for a given entity
	void Entity::GetInitialDataArray(uint8_t* data)
	{
		size_t offset = 0;

		std::memcpy(data + offset, &entityId, sizeof(entityId));
		offset += sizeof(entityId);
		std::memcpy(data + offset, &position, sizeof(position));
		offset += sizeof(position);
		std::memcpy(data + offset, &rotation, sizeof(rotation));
		offset += sizeof(rotation);
		std::memcpy(data + offset, &scale, sizeof(scale));
		offset += sizeof(scale);
		std::memcpy(data + offset, &spawnPosition, sizeof(spawnPosition));
		offset += sizeof(spawnPosition);
		std::memcpy(data + offset, &spawnRotation, sizeof(spawnRotation));
		offset += sizeof(spawnRotation);
		std::memcpy(data + offset, &spawnScale, sizeof(spawnScale));
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
		if (std::memcmp(&position, data + offset, sizeof(position)) != 0)
		{
			std::memcpy(&position, data + offset, sizeof(position));
			SetEntityHasChanged();
			dirty = true;
		}
		offset += sizeof(position);
		if (std::memcmp(&rotation, data + offset, sizeof(rotation)) != 0)
		{
			std::memcpy(&rotation, data + offset, sizeof(rotation));
			SetEntityHasChanged();
			dirty = true;
		}
	}

	// Set the initial data for a given entity
	void Entity::SetInitialDataArray(uint8_t* data)
	{
		size_t offset = 0;
		
		if (std::memcmp(&entityId, data + offset, sizeof(entityId)) != 0)
		{
			std::memcpy(&entityId, data + offset, sizeof(entityId));
			SetEntityHasChanged();
		}
		offset += sizeof(entityId);
		if (std::memcmp(&position, data + offset, sizeof(position)) != 0)
		{
			std::memcpy(&position, data + offset, sizeof(position));
			SetEntityHasChanged();
			dirty = true;
		}
		offset += sizeof(position);
		if (std::memcmp(&rotation, data + offset, sizeof(rotation)) != 0)
		{
			std::memcpy(&rotation, data + offset, sizeof(rotation));
			SetEntityHasChanged();
			dirty = true;
		}
		offset += sizeof(rotation);
		if (std::memcmp(&scale, data + offset, sizeof(scale)) != 0)
		{
			std::memcpy(&scale, data + offset, sizeof(scale));
			SetEntityHasChanged();
			dirty = true;
		}
		offset += sizeof(scale);
		if (std::memcmp(&spawnPosition, data + offset, sizeof(spawnPosition)) != 0)
		{
			std::memcpy(&spawnPosition, data + offset, sizeof(spawnPosition));
			SetEntityHasChanged();
		}
		offset += sizeof(spawnPosition);
		if (std::memcmp(&spawnRotation, data + offset, sizeof(spawnRotation)) != 0)
		{
			std::memcpy(&spawnRotation, data + offset, sizeof(spawnRotation));
			SetEntityHasChanged();
		}
		offset += sizeof(spawnRotation);
		if (std::memcmp(&spawnScale, data + offset, sizeof(spawnScale)) != 0)
		{
			std::memcpy(&spawnScale, data + offset, sizeof(spawnScale));
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
		if (this->rotation != aRotation)
		{
			this->rotation = aRotation;
			this->dirty = true;
			SetEntityHasChanged();
		}
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

	void Entity::SetScale(glm::vec3 scale)
	{
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

	// Spawn State Setters
	void Entity::SetSpawnState(glm::vec3 position, float angInDeg, glm::vec3 axis, float scale)
	{
		this->spawnPosition = position;
		this->spawnRotation = glm::rotate(glm::mat4(1.0f), glm::radians(angInDeg), axis);
		this->spawnScale = glm::vec3(scale, scale, scale);

		SetPosition(position);
		SetRotation(angInDeg, axis);
		SetScale(scale);
	}

	// Reset to spawn state
	void Entity::ResetToSpawnState()
	{
		SetPosition(this->spawnPosition);
		SetRotation(this->spawnRotation);
		SetScale(this->spawnScale);
	}

	// Set component index vector of this entity
	// Only use if null constructor is used
	void Entity::SetComponentIndexArray(std::vector<int> indexList)
	{
		if (indexList.size() != TYPE_COUNT)
			throw("Index list for MakeNewEntity not equal to TYPE_COUNT");
		componentTypeIndexList = indexList;
	}
}

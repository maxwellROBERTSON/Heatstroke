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

// Private Methods

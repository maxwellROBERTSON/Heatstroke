#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>

#include "Component.hpp"
#include "../gltf/glTF.hpp"

class EntityManager;

class Entity
{
public:
	Entity(EntityManager*, int entityId, std::vector<int> typeIndexList);
	~Entity() {};

	// Getters
	int GetEntityId() { return entityId; }
	glm::mat4 GetModelMatrix() { return modelMatrix; }
	int GetComponent(int id) { return componentListId[id]; }
	std::vector<int> GetComponentIndexArray() { return componentListId; }

	// Setters
	void SetModelMatrix(glm::mat4 aModelMatrix) { modelMatrix = aModelMatrix; }

private:
	EntityManager* entityManager;
	int entityId;
	glm::mat4 modelMatrix = glm::mat4(1.0f);

	// Holds a list of components with the type = index and
	// value = component index in that types list
	std::vector<int> componentListId;

};

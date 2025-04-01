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
	glm::mat4 GetModelMatrix();
	int GetComponent(int id) { return componentListId[id]; }
	std::vector<int> GetComponentIndexArray() { return componentListId; }

	// Setters
	void SetPosition(float x, float y, float z);
	void SetPosition(glm::vec3 position);

	void SetRotation(float angInDeg, glm::vec3 axis);
	void SetRotation(glm::quat rotation);

	void SetScale(float xScale, float yScale, float zScale);
	void SetScale(float overallScale);

private:
	EntityManager* entityManager;
	int entityId;

	// Flag to set whether model matrix needs to be updated
	bool dirty = true;
	glm::vec3 position{};
	glm::mat4 rotation{ 1.0f };
	glm::vec3 scale{ 1.0f };

	glm::mat4 modelMatrix = glm::mat4(1.0f);

	// Holds a list of components with the type = index and
	// value = component index in that types list
	std::vector<int> componentListId;

};

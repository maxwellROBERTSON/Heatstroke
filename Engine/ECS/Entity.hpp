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

	int HasComponent(int);
	int GetComponentIndexArray();

private:
	EntityManager* entityManager;
	int entityId;
	bool isActive = true;
	glm::mat4 modelToWorldTransform = glm::mat4(1.0f);

	// Holds a list of components with the type and
	// their id in that types list in EntityManager
	std::vector<int> componentListId;

};

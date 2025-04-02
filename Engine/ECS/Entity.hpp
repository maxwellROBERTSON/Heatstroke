#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <glm/gtc/matrix_transform.hpp>
#include <cstdint>

#include "Components/Component.hpp"
#include "../gltf/glTF.hpp"

namespace Engine
{
	class EntityManager;

	class Entity
	{
	public:
		Entity(EntityManager*, int, std::vector<int>);
		Entity(EntityManager*);
		~Entity() {};

		// Getters

		// Get the size of an entity
		size_t GetEntitySize() { return sizeof(entityId) + sizeof(modelMatrix); }

		// Get the data for a given entity
		void GetDataArray(uint8_t*);

		// Get entity id
		int GetEntityId() { return entityId; }

		// Get model matrix
		glm::mat4 GetModelMatrix() { return modelMatrix; }

		// Get a component index of a given type
		int GetComponent(ComponentTypes t) { return componentTypeIndexList[t]; }

		// Get component index vector of this entity
		std::vector<int> GetComponentIndexArray() { return componentTypeIndexList; }

		// Setters

		// Set the data for a given entity
		void SetDataArray(uint8_t*);

		// Set model matrix
		void SetModelMatrix(glm::mat4 aModelMatrix) { modelMatrix = aModelMatrix; }

		// Set component index vector of this entity
		// Only use if null constructor is used
		void SetComponentIndexArray(std::vector<int> l) { componentTypeIndexList = l; }

	private:
		EntityManager* entityManager;
		int entityId;
		glm::mat4 modelMatrix = glm::mat4(1.0f);

		// Holds a list of components with the type = index and
		// value = component index in that types list
		std::vector<int> componentTypeIndexList;
	};
}

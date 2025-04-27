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
		Entity(EntityManager*);
		Entity(EntityManager*, int, std::vector<int>);
		~Entity() {};

		// Getters

		// Get the size of an entity
		size_t GetEntitySize() { return sizeof(entityId) + sizeof(modelMatrix); }

		// Get the data for a given entity
		void GetDataArray(uint8_t*);

		// Get entity id
		int GetEntityId() { return entityId; }

		// Get model matrix
		glm::mat4 GetModelMatrix();

		// Get a component index of a given type
		int GetComponent(ComponentTypes t) { return componentTypeIndexList[t]; }

		// Get component index vector of this entity
		std::vector<int> GetComponentIndexArray() { return componentTypeIndexList; }

		// Setters

		// Set the data for a given entity
		void SetDataArray(uint8_t*);

		// Position Setters
		void SetPosition(float x, float y, float z);
		void SetPosition(glm::vec3 position);
		glm::vec3 GetPosition() { return position; }

		// Rotation setters
		void SetRotation(float angInDeg, glm::vec3 axis);
		void SetRotation(glm::quat rotation);

		// Scale setters
		void SetScale(float xScale, float yScale, float zScale);
		void SetScale(float overallScale);

		// Set model matrix
		void SetModelMatrix(glm::mat4 aModelMatrix);

		// Set entity has changed in entity manager
		void SetEntityHasChanged();

		// Toggle has changed boolean
		void ToggleHasChanged() { hasChanged = !hasChanged; }

		// Set entity id
		void SetEntityId(int id) { entityId = id; }

		// Set component index vector of this entity
		// Only use if null constructor is used
		void SetComponentIndexArray(std::vector<int> l) { componentTypeIndexList = l; }

	private:
		bool hasChanged = false;
		EntityManager* entityManager;
		int entityId;

		// Flag to set whether model matrix needs to be updated
		bool dirty = false;
		glm::vec3 position{};
		glm::mat4 rotation{ 1.0f };
		glm::vec3 scale{ 1.0f };

		glm::mat4 modelMatrix = glm::mat4(1.0f);

		// Holds a list of components with the type = index and
		// value = component index in that types list
		std::vector<int> componentTypeIndexList;
	};
}

#pragma once

#include <cstdint>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <unordered_map>
#include <vector>

#include "../gltf/glTF.hpp"
#include "Components/Component.hpp"

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
		size_t GetEntitySize() { return sizeof(entityId) + sizeof(position) + sizeof(rotation) + sizeof(scale); }

		// Get the initial size of an entity for initial transfer
		size_t GetInitialEntitySize() { return sizeof(entityId) + 2 * (sizeof(position) + sizeof(rotation) + sizeof(scale)); }

		// Get the data for a given entity
		void GetDataArray(uint8_t*);

		// Get the initial data for a given entity
		void GetInitialDataArray(uint8_t*);

		// Get entity id
		int GetEntityId() { return entityId; }

		// Get model matrix
		glm::mat4 GetModelMatrix();

		// Get a component index of a given type
		int GetComponent(ComponentTypes t) { return componentTypeIndexList[t]; }

		// Get component index vector of this entity
		std::vector<int> GetComponentIndexArray() { return componentTypeIndexList; }

		// Position Getter
		glm::vec3 GetPosition() const { return position; }

		// Rotation Getter
		glm::mat4 GetRotation() const { return rotation; }

		// Scale Getter
		glm::vec3 GetScale() const { return scale; }

		// Setters

		// Set the data for a given entity
		void SetDataArray(uint8_t*);

		// Set the initial data for a given entity
		void SetInitialDataArray(uint8_t*);

		// Set entity has changed in entity manager
		void SetEntityHasChanged();

		// Toggle has changed boolean
		void ToggleHasChanged() { hasChanged = !hasChanged; }

		// Position Setters
		void SetPosition(float x, float y, float z);
		void SetPosition(glm::vec3 position);

		// Rotation setters
		void SetRotation(float angInDeg, glm::vec3 axis);
		void SetRotation(glm::mat4 aRotation); // added in input actions
		void SetRotation(glm::quat rotation);

		// Scale setters
		void SetScale(float xScale, float yScale, float zScale);
		void SetScale(glm::vec3 scale);
		void SetScale(float overallScale);

		// Set model matrix
		void SetModelMatrix(glm::mat4 aModelMatrix);

		// Spawn State Setters
		void SetSpawnState(glm::vec3 position, float angInDeg, glm::vec3 axis, float scale);

		// Reset to spawn state
		void ResetToSpawnState();

		// Set entity id
		void SetEntityId(int id) { entityId = id; }

		// Set component index vector of this entity
		// Only use if null constructor is used
		void SetComponentIndexArray(std::vector<int>);

	private:
		bool hasChanged = false;
		EntityManager* entityManager;
		int entityId;

		// Flag to set whether model matrix needs to be updated
		bool dirty = false;
		glm::vec3 position{ 0.f };
		glm::mat4 rotation{ 1.0f };
		glm::vec3 scale{ 1.0f };

		glm::mat4 modelMatrix = glm::mat4(1.0f);

		// Spawn position, rotation and scale
		glm::vec3 spawnPosition{ 0.f };
		glm::mat4 spawnRotation{ 1.0f };
		glm::vec3 spawnScale{ 1.0f };

		// Holds a list of components with the type = index and
		// value = component index in that types list
		std::vector<int> componentTypeIndexList = std::vector<int>(TYPE_COUNT);
	};
}

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
		Entity(EntityManager*, int, std::vector<int>);
		~Entity() {};

		// Getters

		// Get the size of an entity
		size_t GetEntitySize() { return sizeof(entityId) + sizeof(modelMatrix); }

		// Get the data for a given entity
		void GetData(uint8_t*);

		// Get entity id
		int GetEntityId() { return entityId; }

		// Get model matrix
		glm::mat4 GetModelMatrix();

		// Get a component index of a given type
		int GetComponent(ComponentTypes t) { return componentTypeIndexList[t]; }

		// Get all components of this entity
		std::vector<int> GetComponentIndexArray() { return componentTypeIndexList; }

		// Setters

		// Set the data for a given entity
		void SetData(uint8_t*);

		void SetPosition(float x, float y, float z);
		void SetPosition(glm::vec3 position);

		void SetRotation(float angInDeg, glm::vec3 axis);
		void SetRotation(glm::mat4 aRotation);
		void SetRotation(glm::quat rotation);

		void SetScale(float xScale, float yScale, float zScale);
		void SetScale(float overallScale);

		glm::vec3 GetPosition() const { return position; }
		glm::mat4 GetRotation() const { return rotation; }
		void SetFrontDirection(float cameraYaw);
		glm::vec3 GetFrontDirection() const { return frontDirection; }
		glm::vec3 frontDirection;

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
		std::vector<int> componentTypeIndexList;
	};
}

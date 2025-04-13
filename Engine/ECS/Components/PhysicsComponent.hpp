#pragma once

#include <string>
#include "Component.hpp"
#include "PxPhysicsAPI.h"
#include "../Physics/PhysicsWorld.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

#include "../../gltf/Model.hpp"

#ifdef OS_LINUX
#include <cassert>
#endif

#include "../EntityManager.hpp"

namespace Engine
{
	class EntityManager;
	class Entity;
}

namespace Engine
{
	using namespace physx;

	class PhysicsWorld;

	class PhysicsComponent : public Component<PhysicsComponent>
	{
	public:
		// three physics objects for now
		enum class PhysicsType {
			STATIC,
			DYNAMIC,
			CONTROLLER
			//...
		};

		// Constructors

		PhysicsComponent() : entityManager(nullptr), entity(nullptr) {}
		PhysicsComponent(Engine::EntityManager* entityManager, Engine::Entity* entity) : entityManager(entityManager), entity(entity) {}

		virtual void operator=(const PhysicsComponent& other) override
		{
			this->staticBody = other.staticBody;
			this->dynamicBody = other.dynamicBody;
			this->controller = other.controller;
			this->type = other.type;
			this->scale = other.scale;
			this->isPerson = other.isPerson;
			this->entityId = other.entityId;
		}

		// Getters

		// Static type getter from Component parent
		ComponentTypes static StaticType() { return ComponentTypes::PHYSICS; }

		// Static size getter from Component parent
		size_t static StaticSize() { return sizeof(type) + sizeof(translation) + sizeof(scale) + sizeof(rotation) + sizeof(isPerson) + sizeof(entityId); }

		// Get component data
		void GetDataArray(uint8_t*) override;

		// Get physics object type STATIC/DYNAMIC/CONTROLLER
		PhysicsType GetPhysicsType() { return type; }

		// Get translation vector
		glm::vec3 GetTranslation() { return translation; }

		// Get scale vector
		glm::vec3 GetScale() { return scale; }

		// Get rotation quaternion
		glm::quat GetRotation() { return rotation; }

		// Get static body pointer
		PxRigidStatic* GetStaticBody() { return staticBody; }

		// Get dynamic body pointer
		PxRigidDynamic* GetDynamicBody() { return dynamicBody; }

		// Get controller pointer
		PxController* GetController() { return controller; }

		// Get object is a person
		int GetIsPerson() { return isPerson; }

		// Get entity id
		int GetEntityId() { return entityId; }

		// Setters

		// Set component data
		void SetDataArray(uint8_t*) override;

		// Init
		void Init(PhysicsWorld&, PhysicsType, vk::Model&, glm::mat4, int);

		// Init complex shape
		void InitComplexShape(PhysicsWorld&, PhysicsType, vk::Model&, glm::mat4, int);

		// Set physics object type STATIC/DYNAMIC/CONTROLLER
		void SetPhysicsType(PhysicsType t) { type = t; SetComponentHasChanged(); }

		// Set scale vector
		void SetScale(glm::vec3 s) { scale = s; SetComponentHasChanged(); }

		// Set rotation quaternion
		void SetRotation(glm::quat r) { rotation = r; SetComponentHasChanged(); }

		// Set object is a person
		void SetIsPerson(bool aIsPerson) { isPerson = aIsPerson; SetComponentHasChanged(); }

		// Set component has changed in entity manager
		void SetComponentHasChanged();

		// Toggle has changed boolean
		void ToggleHasChanged() { hasChanged = !hasChanged; }

	private:
		// EntityManager pointer
		Engine::EntityManager* entityManager;
		// Entity pointer
		Engine::Entity* entity;

		// If component has changed since last network update
		bool hasChanged = false;

		PxRigidStatic* staticBody = nullptr;
		PxRigidDynamic* dynamicBody = nullptr;
		PxController* controller = nullptr;

		PhysicsType type = PhysicsType::STATIC;

		glm::vec3 translation = glm::vec3(0, 0, 0);
		glm::vec3 scale = glm::vec3(0, 0, 0);
		glm::quat rotation = glm::angleAxis(0.f, glm::vec3(0, 0, 0));

		bool isPerson = false;

		int entityId = -1;

		bool DecomposeTransform(const glm::mat4&, glm::vec3&, glm::quat&, glm::vec3&);
	};
}

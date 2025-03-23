#pragma once

#include <string>
#include "Component.hpp"
#include "PxPhysicsAPI.h"
#include "../Physics/PhysicsWorld.hpp"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using namespace physx;

class PhysicsComponent : public Component<PhysicsComponent>
{
public:
	PxRigidStatic* staticBody = nullptr;
	PxRigidDynamic* dynamicBody = nullptr;
	PxController* controller = nullptr;
	// three physics objects for now
	enum class PhysicsType {
		STATIC,
		DYNAMIC,
		CONTROLLER
		//...
	};

	PhysicsType type;
	glm::vec3 scale;
	glm::quat rotation = glm::quat(1, 0, 0, 0);

	PhysicsComponent() {};

	void init(PhysicsWorld& pworld,PhysicsType physicsType, glm::mat4& transform, int index) {

		entityId = index;

		// parse mat4
		glm::vec3 translation;
		glm::quat rotation;
		if (!DecomposeTransform(transform, translation, rotation, scale)) {
			std::cout << "DecomposeTransform failed!" << std::endl;
			return;
		}
		scale.x = glm::length(glm::vec3(transform[0]));
		scale.y = glm::length(glm::vec3(transform[1]));
		scale.z = glm::length(glm::vec3(transform[2]));


		PxTransform pxTransform(
			PxVec3(translation.x, translation.y, translation.z),
			PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
		);
		PxMaterial* material = pworld.gPhysics->createMaterial(0.5f, 0.5f, 0.5f);
		material->setRestitution(0.0f);
		type = physicsType;

		switch (type)
		{
			case PhysicsType::STATIC:
			{
				staticBody = pworld.gPhysics->createRigidStatic(pxTransform);
				if (staticBody) {
					PxShape* shape = PxRigidActorExt::createExclusiveShape(
						*staticBody, PxBoxGeometry(scale.x, scale.y, scale.z), *material
					);

					pworld.gScene->addActor(*staticBody);

				}
				break;
			}

			case PhysicsType::DYNAMIC:
			{
				dynamicBody = pworld.gPhysics->createRigidDynamic(pxTransform);

				if (dynamicBody) {
					PxShape* shape = PxRigidActorExt::createExclusiveShape(
						*dynamicBody, PxBoxGeometry(scale.x, scale.y, scale.z), *material
					);

					pworld.gScene->addActor(*dynamicBody);
					pworld.numDynamicRigid++;
				}
				break;
			}

			case PhysicsType::CONTROLLER:
			{
				// set ControllerDescription
				PxCapsuleControllerDesc desc;
				desc.height = 1.f;
				desc.radius = 0.3f;
				desc.stepOffset = 0.1f;
				//desc.contactOffset
				desc.material = pworld.gPhysics->createMaterial(0.5f, 0.5f, 0.6f);
				desc.position = PxExtendedVec3(translation.x, translation.y + desc.height / 2 + desc.radius, translation.z);
				desc.slopeLimit = 0.8f;
				desc.upDirection = PxVec3(0, 1, 0);

				PxCapsuleController* pcontroller = static_cast<PxCapsuleController*>(pworld.gControllerManager->createController(desc));
				controller = pcontroller;
				pworld.controller = pcontroller;

				break;
			}
		}
	}


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
	int GetIsPerson() { return isPerson; }
	//int GetModelIndex() { return modelIndex; }
	int GetEntityId() { return entityId; }

	// Setters
	void SetIsPerson(bool aIsPerson) { isPerson = aIsPerson; }
	//void SetModelIndex(int index) { modelIndex = index; }



private:

	bool isPerson = false;

	int entityId = -1;

	bool DecomposeTransform(const glm::mat4& matrix, glm::vec3& translation, glm::quat& rotation, glm::vec3& scale)
	{
		glm::vec3 skew;
		glm::vec4 perspective;
		bool success = glm::decompose(matrix, scale, rotation, translation, skew, perspective);
		if (success) {
			rotation = glm::conjugate(rotation);
		}
		return success;
	}

};



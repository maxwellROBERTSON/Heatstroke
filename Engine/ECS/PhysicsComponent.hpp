#pragma once

#include <string>
#include "Component.hpp"
#include "PxPhysicsAPI.h"
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

using namespace physx;

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

	PhysicsType type;



	// constructor
	PhysicsComponent() {};

	// destructor
	~PhysicsComponent() {
		//if (staticBody) staticBody->release();
		//if (dynamicBody) dynamicBody->release();
		//if (controller) controller->release();
	}

	void init(PxPhysics*& physics, PxScene*& scene, PhysicsType physicsType, glm::mat4& transform) {

		// parse mat4
		glm::vec3 translation;
		glm::quat rotation;
		glm::vec3 scale;
		if (!DecomposeTransform(transform, translation, rotation, scale)) {
			std::cout << "DecomposeTransform failed!" << std::endl;
			return;
		}
		PxTransform pxTransform(
			PxVec3(translation.x, translation.y, translation.z),
			PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
		);
						PxMaterial* material = physics->createMaterial(0.5f, 0.5f, 0.5f);

		// TODO: apply scale to the object
		type = physicsType;

		switch (type) {
		case PhysicsType::STATIC:
		{
			staticBody = physics->createRigidStatic(pxTransform);
			if (staticBody) {
				PxShape* shape = PxRigidActorExt::createExclusiveShape(
					*staticBody, PxBoxGeometry(0.2f, 0.2f, 0.2f), *material
				);

				scene->addActor(*staticBody);
			}
			break;
		}
		case PhysicsType::DYNAMIC:
			dynamicBody = physics->createRigidDynamic(pxTransform);
			if (dynamicBody) scene->addActor(*dynamicBody);
			break;
		case PhysicsType::CONTROLLER:
			// TODO:CharacterController, descriptor...
			break;
		}
	}


	virtual void operator=(const PhysicsComponent& other) override
	{
		this->isPerson = other.isPerson;
	}

	// Getters
	int GetIsPerson() { return isPerson; }

	// get physics component position
	PxVec3 GetPosition() const {
		switch (type) {
		case PhysicsType::DYNAMIC:
			return dynamicBody ? dynamicBody->getGlobalPose().p : PxVec3(0, 0, 0);
		case PhysicsType::STATIC:
			return staticBody ? staticBody->getGlobalPose().p : PxVec3(0, 0, 0);
		case PhysicsType::CONTROLLER:
			return controller ? PxVec3(controller->getPosition().x, controller->getPosition().y, controller->getPosition().z) : PxVec3(0, 0, 0);
		default:
			return PxVec3(0, 0, 0);
		}
	}

	// Setters
	void SetIsPerson(bool aIsPerson) { isPerson = aIsPerson; }

private:
	PxRigidStatic* staticBody = nullptr;
	PxRigidDynamic* dynamicBody = nullptr;
	PxController* controller = nullptr;
	bool isPerson = false;

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



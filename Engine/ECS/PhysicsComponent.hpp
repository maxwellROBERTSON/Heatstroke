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



	// constructor
	PhysicsComponent() {};

	// destructor
	~PhysicsComponent() {}

	void init(PhysicsWorld& pworld,PhysicsType physicsType, glm::mat4& transform) {

		//PxPhysics*& physics;
		//PxScene*& scene,
		// 
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
		PxMaterial* material = pworld.gPhysics->createMaterial(0.5f, 0.5f, 0.5f);

		// TODO: apply scale to the object
		type = physicsType;

		switch (type) {
		case PhysicsType::STATIC:
		{
			staticBody = pworld.gPhysics->createRigidStatic(pxTransform);
			if (staticBody) {
				PxShape* shape = PxRigidActorExt::createExclusiveShape(
					*staticBody, PxBoxGeometry(0.2f, 0.2f, 0.2f), *material
				);

				pworld.gScene->addActor(*staticBody);
				
			}
			break;
		}
		case PhysicsType::DYNAMIC:
			dynamicBody = pworld.gPhysics->createRigidDynamic(pxTransform);
			
			if (dynamicBody) {
				PxShape* shape = PxRigidActorExt::createExclusiveShape(
					*dynamicBody, PxBoxGeometry(0.2f, 0.2f, 0.2f), *material
				);

				pworld.gScene->addActor(*dynamicBody);
				pworld.numDynamicRigid++;
			}
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
	int GetModelIndex() { return modelIndex; }
	// get physics component position
	glm::mat4 GetTrans() const {
		//switch (type) {
		//case PhysicsType::DYNAMIC:
		//	return dynamicBody ? dynamicBody->getGlobalPose().p : PxVec3(0, 0, 0);
		//case PhysicsType::STATIC:
		//	return staticBody ? staticBody->getGlobalPose().p : PxVec3(0, 0, 0);
		//case PhysicsType::CONTROLLER:
		//	return controller ? PxVec3(controller->getPosition().x, controller->getPosition().y, controller->getPosition().z) : PxVec3(0, 0, 0);
		//default:
		//	return PxVec3(0, 0, 0);

			return glm::mat4();
	}

	// Setters
	void SetIsPerson(bool aIsPerson) { isPerson = aIsPerson; }
	void SetModelIndex(int index) { modelIndex = index; }

	//void updateObjects() {
	//	std::pair<void*, int> renderComponents = entityManager.GetComponents<RenderComponent>();

	//}

private:

	bool isPerson = false;

	int modelIndex = -1;


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



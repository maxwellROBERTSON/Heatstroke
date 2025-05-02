#pragma once

#include "PxPhysicsAPI.h"
#include <iostream>
#if defined(_WIN32)
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif
#include <vector>

#include "../gltf/Model.hpp"

#include "../ECS/EntityManager.hpp"
#include "../ECS/Entity.hpp"

namespace Engine
{
	using namespace physx;

	class EntityManager;

	class PhysicsWorld {
	public:
		std::vector<PxRigidStatic*> staticBodies;
		PxFoundation* gFoundation = nullptr;
		PxPhysics* gPhysics = nullptr;
		PxScene* gScene = nullptr;
		PxControllerManager* gControllerManager = nullptr;
		PxDefaultAllocator gAllocator;
		static PxDefaultErrorCallback gErrorCallback;
		PxMaterial* gMaterial = nullptr;
		PxCapsuleController* controller = nullptr;
		PxU32 numDynamicRigid = 0;

		// CapsuleController
		PxCapsuleController* gCapsuleController = nullptr;
		float verticalVelocity = 0.0f;

		// PVD
#if defined(_WIN32)
		PxPvd* gPvd = nullptr;
		PxPvdTransport* gTransport = nullptr;
#endif

		PxReal gSimulationTimer = 0.0f;
		PxReal gTimestep = 1.0f / 60.0f; // 60 FPS

		void init(EntityManager*);

		void setControllerEntity(Entity*);
		void setControllerHeight(float);
		float getControllerHeight() { return controllerHeight; }

		void updatePhysics(PxReal timeDelta, bool updateCharacter);

		void updateCharacter(PxReal deltatime);

		void handleMovement(PxReal deltatime);

		PxRaycastHit handleShooting();

		void updateObjects(std::vector<Engine::vk::Model>& models); //TODO

		void cleanupPhysX();

		glm::mat4 ConvertPxTransformToGlmMat4(const PxTransform& transform);

		void DebugDrawRayInPVD(PxScene* scene, const PxVec3& start, const PxVec3& end, PxU32 color);

	private:
		EntityManager* entityManager;
		Entity* controllerEntity;
		float controllerHeight = 0.f;
	};
}

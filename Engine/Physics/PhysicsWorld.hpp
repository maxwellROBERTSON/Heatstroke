#pragma once

#include "PxPhysicsAPI.h"
#include <iostream>
#include <conio.h>
#include <vector>
#include "../ECS/EntityManager.hpp"
using namespace physx;

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

	// PVD
	PxPvd* gPvd = nullptr;
	PxPvdTransport* gTransport = nullptr;


	PxReal gTimestep = 1.0f / 60.0f; // 60 FPS

	void init();

	void updateCharacter(PxReal deltatime);

	void updateObjects(EntityManager& entityManager, std::vector<Engine::vk::Model>& models);

	void cleanupPhysX();

	glm::mat4 ConvertPxTransformToGlmMat4(const PxTransform& transform);

private:

};

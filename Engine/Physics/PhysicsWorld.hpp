#pragma once

#include "PxPhysicsAPI.h"
//#include "physx/PxPhysicsAPI.h"
//#include "PxPhysicsAPI.h"
#include <iostream>
#include <conio.h>

using namespace physx;

class PhysicsWorld {
public:
	PxFoundation* gFoundation = nullptr;
	PxPhysics* gPhysics = nullptr;
	PxScene* gScene = nullptr;
	PxControllerManager* gControllerManager = nullptr;
	PxDefaultAllocator gAllocator;
	static PxDefaultErrorCallback gErrorCallback;
	PxMaterial* gMaterial = nullptr;

	// CapsuleController
	PxCapsuleController* gCapsuleController = nullptr;

	// PVD
	PxPvd* gPvd = nullptr;
	PxPvdTransport* gTransport = nullptr;


	PxReal gTimestep = 1.0f / 60.0f; // 60 FPS

	void init();

	void createCapsuleController();

	void createStaticBox();

	void cleanupPhysX();

};

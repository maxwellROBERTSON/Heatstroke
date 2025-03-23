#include "PhysicsWorld.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../vulkan/objects/Model.hpp"
#include "../ECS/EntityManager.hpp"
#include "../ECS/PhysicsComponent.hpp"
physx::PxDefaultErrorCallback PhysicsWorld::gErrorCallback;

void PhysicsWorld::init() {

	// gFoundation
	gFoundation = PxCreateFoundation(PX_PHYSICS_VERSION, gAllocator, gErrorCallback);
	if (!gFoundation)
	{
		std::cerr << "PxCreateFoundation failed!" << std::endl;
		std::exit(-1);
	}

	// Pvd
	gPvd = PxCreatePvd(*gFoundation);
	if (!gPvd)
	{
		std::cerr << "PxCreatePvd failed!" << std::endl;
		std::exit(-1);
	}
	gTransport = PxDefaultPvdSocketTransportCreate("127.0.0.1", 5425, 10);
	if (!gTransport)
	{
		std::cerr << "PxDefaultPvdSocketTransportCreate failed!" << std::endl;
		std::exit(-1);
	}
	//bool isConnected = gPvd->connect(*gTransport, PxPvdInstrumentationFlag::eALL);
	bool isConnected = gPvd->connect(*gTransport, PxPvdInstrumentationFlag::eDEBUG);

	if (!isConnected)
	{
		std::cerr << "PVD not connected\n";
	}

	gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, gPvd);
	if (!gPhysics)
	{
		std::cerr << "PxCreatePhysics failed!" << std::endl;
		std::exit(-1);
	}

	// create Scene
	PxSceneDesc sceneDesc(gPhysics->getTolerancesScale());
	sceneDesc.gravity = PxVec3(0.0f, -9.81f, 0.0f);

	PxDefaultCpuDispatcher* mCpuDispatcher = PxDefaultCpuDispatcherCreate(1);
	if (!mCpuDispatcher)
		std::cerr << "PxDefaultCpuDispatcherCreate failed!" << std::endl;

	sceneDesc.cpuDispatcher = mCpuDispatcher;
	sceneDesc.filterShader = PxDefaultSimulationFilterShader;
	gScene = gPhysics->createScene(sceneDesc);
	if (!gScene)
	{
		std::cerr << "createScene failed!" << std::endl;
		std::exit(-1);
	}
	// set parameters for the scene
	gScene->setVisualizationParameter(PxVisualizationParameter::eSCALE, 1.0f);
	//gScene->setVisualizationParameter(PxVisualizationParameter::eCOLLISION_SHAPES, 1.0f);

	// material
	gMaterial = gPhysics->createMaterial(0.5f, 0.5f, 0.5f);
	if (!gMaterial)
	{
		std::cerr << "createMaterial failed!" << std::endl;
		std::exit(-1);
	}


	// testplane!
	
	//PxRigidStatic* groundPlane = PxCreatePlane(*gPhysics, PxPlane(0, 1, 0, 0), *gMaterial);
	//gScene->addActor(*groundPlane);
	

}

// update models matrices
void PhysicsWorld::updateObjects(EntityManager& entityManager, std::vector<Engine::vk::Model>& models)
{
	// get all PhysicsComponent
	std::pair<void*, int> physicsComponents = entityManager.GetComponents<PhysicsComponent>();
	for (std::size_t i = 0; i < physicsComponents.second; i++) {
		PhysicsComponent p = reinterpret_cast<PhysicsComponent*>(physicsComponents.first)[i];
		// only update dynamic now!
		if (p.type == PhysicsComponent::PhysicsType::DYNAMIC)
		{
			glm::mat4 matrix = ConvertPxTransformToGlmMat4(p.dynamicBody->getGlobalPose());
			matrix = glm::scale(matrix, p.scale);
			entityManager.GetEntity(p.GetEntityId())->SetModelMatrix(matrix);
		}
	}
}

void PhysicsWorld::createCapsuleController()
{
	PxCapsuleControllerDesc desc;
	desc.radius = 0.5f;
	desc.height = 1.8f;
	desc.material = gMaterial;
	desc.contactOffset = 0.1f;
	desc.position = PxExtendedVec3(0.0, 0.0, desc.height / 2.f + desc.contactOffset + desc.radius);
	desc.slopeLimit = 0.0f;
	desc.stepOffset = 0.5f;
	desc.upDirection = PxVec3(0, 0, 1);

	gCapsuleController = static_cast<PxCapsuleController*>(gControllerManager->createController(desc));
	if (!gCapsuleController)
	{
		std::cerr << "createCapsuleController failed!" << std::endl;
		std::exit(-1);
	}
}

void PhysicsWorld::cleanupPhysX()
{
	if (gControllerManager)
	{
		gControllerManager->release();
		gControllerManager = nullptr;
	}

	if (gScene)
	{
		gScene->release();
		gScene = nullptr;
	}

	if (gPhysics)
	{
		gPhysics->release();
		gPhysics = nullptr;
	}

	// disconnect PVD
	if (gPvd)
	{
		PxPvdTransport* transport = gPvd->getTransport();
		if (transport)
		{
			transport->disconnect();
			transport->release();
		}
		gPvd->release();
		gPvd = nullptr;
	}

	if (gFoundation)
	{
		gFoundation->release();
		gFoundation = nullptr;
	}
}

void PhysicsWorld::createStaticBox()
{
	PxVec3 pos = PxVec3(2.0f, 2.0f, 1.f);
	float angle = PxPi * 0.25f; // 45 degrees
	PxQuat rotation(angle, PxVec3(0.f, 0.f, 1.f)); // (angle, axis)

	PxTransform boxTransform(pos, rotation);

	PxRigidStatic* boxActor = gPhysics->createRigidStatic(boxTransform);
	if (!boxActor)
	{
		std::cerr << "createRigidStatic for box failed!\n";
		return;
	}

	PxBoxGeometry boxGeom(1.0f, 1.0f, 1.0f);

	//
	PxShape* boxShape = PxRigidActorExt::createExclusiveShape(*boxActor, boxGeom, *gMaterial);

	if (!boxShape)
	{
		std::cerr << "createExclusiveShape for box failed!\n";
		return;
	}

	// add actor to the scene
	gScene->addActor(*boxActor);


}

glm::mat4 PhysicsWorld::ConvertPxTransformToGlmMat4(const PxTransform& transform) {

	glm::quat rotation(transform.q.w, transform.q.x, transform.q.y, transform.q.z);

	glm::vec3 translation(transform.p.x, transform.p.y, transform.p.z);

	glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

	rotationMatrix[3] = glm::vec4(translation, 1.0f);

	return rotationMatrix;
}
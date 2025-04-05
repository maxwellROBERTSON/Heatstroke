#include "PhysicsWorld.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/quaternion.hpp>
#include "../gltf/Model.hpp"
#include "../ECS/EntityManager.hpp"
#include "../ECS/Components/PhysicsComponent.hpp"
#include "../Input/Keyboard.hpp"
#include "../Input/InputCodes.hpp"
#include "../Input/Input.hpp"

namespace Engine
{
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
#if defined(_WIN32)
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
#else
		gPhysics = PxCreatePhysics(PX_PHYSICS_VERSION, *gFoundation, PxTolerancesScale(), true, NULL);
#endif
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

		gControllerManager = PxCreateControllerManager(*gScene);

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
	}

	void PhysicsWorld::updatePhysics(PxReal timeDelta) {
		// Previously, when we didn't have the option of turning VSync off,
		// the timeDelta was 1/x where x was the refresh rate of the users
		// monitor, which most of the time was slightly less than 0.016f (1/60).
		// Since we now give the option of turning off vsync, the frame time
		// can be much much smaller and more volatile and running gScene->simulate()
		// every one of those frames can lead to unstable looking physics.
		// So we use a separate simulation timer and only update the physics
		// when 1/60th of a second has passed. This is obviously reliant on
		// the fact we can run the engine and game faster than 60 fps.

		// (Note, having the physics timestep be 1/60 can sometimes look a bit
		// jittery, so we could use a smaller timestep to get smoother
		// physics and character movement or possible decrease how much a character
		// should move per timestep in updateCharacter())
		this->gSimulationTimer += timeDelta;

		if (this->gSimulationTimer > gTimestep) {
			this->updateCharacter(gTimestep);
			this->gScene->simulate(gTimestep);
			this->gScene->fetchResults(true);
			this->gSimulationTimer -= gTimestep;
		}
	}

	void PhysicsWorld::updateCharacter(PxReal deltatime)
	{
		//if (this->controller)
		//{
		//	PxVec3 displacement(0.0f, -9.81f * deltatime, 0.0f);
		//	PxControllerFilters filters;
		//	this->controller->move(displacement, 0.01f, deltatime, filters);

		//}
		if (this->controller)
		{
			PxVec3 displacement(0.0f, -9.81f * deltatime, 0.0f);
			PxVec3 old = displacement;
			float speed = 5.0f;

			//auto& keys = Engine::Keyboard::getKeyStates();
			auto& keyboard = Engine::InputManager::getKeyboard();

			if (keyboard.isPressed(HS_KEY_W)) {
				displacement.z -= speed * deltatime;
			}
			if (keyboard.isPressed(HS_KEY_S)) {
				displacement.z += speed * deltatime;
			}
			if (keyboard.isPressed(HS_KEY_A)) {
				displacement.x -= speed * deltatime;
			}
			if (keyboard.isPressed(HS_KEY_D)) {
				displacement.x += speed * deltatime;
			}

			// Only run PxController::move() if we actually moved, since this
			// method is quite expensive to run every frame if we are not moving
			if (old != displacement) {
				PxControllerFilters filters;
				this->controller->move(displacement, 0.01f, deltatime, filters);
			}

		}

	}

	// update models matrices
	void PhysicsWorld::updateObjects(Engine::EntityManager& entityManager, std::vector<Engine::vk::Model>& models)
	{

		// get all PhysicsComponent
		std::vector<std::unique_ptr<ComponentBase>>* physicsComponents = entityManager.GetComponentsOfType(PHYSICS);
		for (std::size_t i = 0; i < physicsComponents->size(); i++) {
			PhysicsComponent* p = reinterpret_cast<PhysicsComponent*>((*physicsComponents)[i].get());
			//glm::mat4 matrix(1.0f);
			// dynamic update
			if (p->type == PhysicsComponent::PhysicsType::DYNAMIC)
			{
				Entity* entity = entityManager.GetEntity(p->GetEntityId());

				PxTransform transform = p->dynamicBody->getGlobalPose();
				entity->SetPosition(transform.p.x, transform.p.y, transform.p.z);
				entity->SetRotation(glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z));
				entity->SetScale(p->scale.x, p->scale.y, p->scale.z);

				continue;
			}

			// controller update
			if (p->type == PhysicsComponent::PhysicsType::CONTROLLER)
			{
				PxExtendedVec3 pos = p->controller->getFootPosition();
				entityManager.GetEntity(p->GetEntityId())->SetPosition(pos.x, pos.y, pos.z);
			}
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
#if defined(_WIN32)
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
#endif

		if (gFoundation)
		{
			gFoundation->release();
			gFoundation = nullptr;
		}
	}

	glm::mat4 PhysicsWorld::ConvertPxTransformToGlmMat4(const PxTransform& transform) {

		glm::quat rotation(transform.q.w, transform.q.x, transform.q.y, transform.q.z);

		glm::vec3 translation(transform.p.x, transform.p.y, transform.p.z);

		glm::mat4 rotationMatrix = glm::mat4_cast(rotation);

		rotationMatrix[3] = glm::vec4(translation, 1.0f);

		return rotationMatrix;
	}
}

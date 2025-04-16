#include "../ECS/Components/PhysicsComponent.hpp"
#include "../ECS/EntityManager.hpp"
#include "../gltf/Model.hpp"
#include "../Input/Input.hpp"
#include "../Input/InputCodes.hpp"
#include "../Input/Keyboard.hpp"
#include "PhysicsWorld.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

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

	void PhysicsWorld::updateCharacter(Entity* playerEntity, PxReal deltatime)
	{
		//if (this->controller)
		//{
		//	PxVec3 displacement(0.0f, -9.81f * deltatime, 0.0f);
		//	PxControllerFilters filters;
		//	this->controller->move(displacement, 0.01f, deltatime, filters);

		//}
		glm::vec3 entityFrontDir = playerEntity->frontDirection;
		glm::vec3 entityRightDir = glm::normalize(glm::cross(entityFrontDir, glm::vec3(0.0f, 1.0f, 0.0f)));

		if (this->controller)
		{
			//PxVec3 displacement(0.0f, -9.81f * deltatime, 0.0f);
			PxVec3 displacement(0.0f, 0.0f * deltatime, 0.0f);
			float speed = 5.0f;
			PxVec3 frontDir(entityFrontDir.x, entityFrontDir.y, entityFrontDir.z);
			PxVec3 rightDir(entityRightDir.x, entityRightDir.y, entityRightDir.z);

			auto& keyboard = Engine::InputManager::getKeyboard();

			if (keyboard.isPressed(HS_KEY_W)) {
				//displacement.z += speed * deltatime;
				displacement += frontDir * speed * deltatime;
				//displacement.z += speed * deltatime;
			}
			if (keyboard.isPressed(HS_KEY_S)) {
				displacement -= frontDir * speed * deltatime;


				//displacement.z -= speed * deltatime;
			}
			if (keyboard.isPressed(HS_KEY_A)) {
				displacement -= rightDir * speed * deltatime;

				//displacement.x += speed * deltatime;
			}
			if (keyboard.isPressed(HS_KEY_D)) {
				//displacement.x -= speed * deltatime;
				displacement += rightDir * speed * deltatime;

			}

			PxControllerFilters filters;
			this->controller->move(displacement, 0.01f, deltatime, filters);
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

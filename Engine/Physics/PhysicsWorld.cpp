#include "../../Game/DemoGame.hpp"
#include "../Core/Game.hpp"
#include "../ECS/Components/AudioComponent.hpp"
#include "../ECS/Components/PhysicsComponent.hpp"
#include "../ECS/EntityManager.hpp"
#include "../gltf/Model.hpp"
#include "../Input/Input.hpp"
#include "../Input/InputCodes.hpp"
#include "../Input/Keyboard.hpp"
#include "IgnoreSelfFilterCallback.hpp"
#include "PhysicsWorld.hpp"
#include "RaycastUtility.hpp"
#include "RaycastUtility.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/string_cast.hpp>

namespace Engine
{
	physx::PxDefaultErrorCallback PhysicsWorld::gErrorCallback;

	void PhysicsWorld::init(EntityManager* entityManager) {

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

		this->entityManager = entityManager;
	}

	void PhysicsWorld::updatePhysics(Entity* playerEntity, PxReal timeDelta)
	{
		this->gTimestep = std::clamp(timeDelta, 1.0f / 240.0f, 1.0f / 60.0f);

		this->gSimulationTimer += timeDelta;

		if (this->gSimulationTimer > gTimestep) {
			if (playerEntity != nullptr)
				this->updateCharacter(playerEntity, gTimestep);
			this->gScene->simulate(gTimestep);
			this->gScene->fetchResults(true);
			this->gSimulationTimer -= gTimestep;
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
		// when our physics timestep has passed. This is obviously reliant on
		// the fact we can run the engine and game faster than our timestep
		// minimum of 1/60th of a second.

		// (Note, we use a timestep based on the time delta between 240 fps and
		// 60 fps, ideally time delta is less than 1/240 so we get the smoothest
		// character movement, but some hardware may compute a frame slower
		// and as such, use a bigger timestep, resulting in inconsistent physics. 
		// If someone has a better way to get smooth character movement then 
		// please update this method)
		this->gTimestep = std::clamp(timeDelta, 1.0f / 240.0f, 1.0f / 60.0f);

		this->gSimulationTimer += timeDelta;

		if (this->gSimulationTimer > gTimestep) {
			this->updateCharacter(gTimestep);
			this->gScene->simulate(gTimestep);
			this->gScene->fetchResults(true);
			this->gSimulationTimer -= gTimestep;
		}
	}

	void PhysicsWorld::updateCharacter(PxReal deltatime) {
		//handleMovement(playerEntity, deltatime);
		handleMovement(deltatime);
		//handleShooting();
	}

	void PhysicsWorld::updateCharacter(Entity* playerEntity, PxReal deltatime)
	{
		handleMovement(playerEntity, deltatime);
		//handleShooting();
	}

	void PhysicsWorld::handleMovement(PxReal deltatime)
	{
		if (this->controller)
		{

			PxVec3 displacement(0.0f, -9.81f * deltatime, 0.0f);
			PxVec3 old = displacement;
			float speed = 5.0f;
			const float jumpSpeed = 3.0f;
			const float gravity = -9.81f;

			auto& keyboard = Engine::InputManager::getKeyboard();

			if (keyboard.isPressed(HS_KEY_W)) {
				displacement.x -= speed * deltatime;
			}
			if (keyboard.isPressed(HS_KEY_S)) {
				displacement.x += speed * deltatime;
			}
			if (keyboard.isPressed(HS_KEY_D)) {
				displacement.z -= speed * deltatime;
			}
			if (keyboard.isPressed(HS_KEY_A)) {
				displacement.z += speed * deltatime;
			}

			// isGrounded check
			PxControllerState cstate;
			controller->getState(cstate);
			bool isGrounded = (cstate.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN);

			if (isGrounded && keyboard.isPressed(HS_KEY_SPACE)) {
				verticalVelocity = jumpSpeed;
			}
			verticalVelocity += gravity * deltatime;
			displacement.y = verticalVelocity * deltatime;

			// Only run PxController::move() if we actually moved, since this
			// method is quite expensive to run every frame if we are not moving
			if (old != displacement) {
				PxControllerFilters filters;
				this->controller->move(displacement, 0.01f, deltatime, filters);
			}

			// reset verticalVelocity
			if (isGrounded && verticalVelocity < 0.0f) {
				verticalVelocity = 0.0f;
			}

		}
	}

	void PhysicsWorld::handleMovement(Entity* playerEntity, PxReal deltatime)
	{
		glm::vec3 entityFrontDir = playerEntity->frontDirection;
		glm::vec3 entityRightDir = glm::normalize(glm::cross(entityFrontDir, glm::vec3(0.0f, 1.0f, 0.0f)));
		if (this->controller)
		{
			//const float gravity = -9.81f;
			const float gravity = 0.0f;
			PxVec3 displacement(0.0f, gravity * deltatime, 0.0f);
			PxVec3 old = displacement;
			float speed = 5.0f;
			const float jumpSpeed = 3.0f;
			PxVec3 frontDir(entityFrontDir.x, entityFrontDir.y, entityFrontDir.z);
			PxVec3 rightDir(entityRightDir.x, entityRightDir.y, entityRightDir.z);

			auto& keyboard = Engine::InputManager::getKeyboard();

			if (keyboard.isPressed(HS_KEY_W)) {
				displacement += frontDir * speed * deltatime;
			}

			if (keyboard.isPressed(HS_KEY_S)) {
				displacement -= frontDir * speed * deltatime;
			}

			if (keyboard.isPressed(HS_KEY_A)) {
				displacement -= rightDir * speed * deltatime;
			}

			if (keyboard.isPressed(HS_KEY_D)) {
				displacement += rightDir * speed * deltatime;
			}

			// isGrounded check
			PxControllerState cstate;
			controller->getState(cstate);
			bool isGrounded = (cstate.collisionFlags & PxControllerCollisionFlag::eCOLLISION_DOWN);


			if (isGrounded && keyboard.isPressed(HS_KEY_SPACE)) {
				verticalVelocity = jumpSpeed;
			}
			verticalVelocity += gravity * deltatime;
			displacement.y = verticalVelocity * deltatime;

			// Only run PxController::move() if we actually moved, since this
			// method is quite expensive to run every frame if we are not moving
			if (old != displacement) {
				PxControllerFilters filters;
				this->controller->move(displacement, 0.01f, deltatime, filters);
			}

			// reset verticalVelocity
			if (isGrounded && verticalVelocity < 0.0f) {
				verticalVelocity = 0.0f;
			}

		}
	}

	void PhysicsWorld::handleShooting() {
		//auto& keyboard = Engine::InputManager::getKeyboard();
		////auto& mouse = Engine::InputManager::getMouse();
		//if (keyboard.isPressed(HS_KEY_P)) {

			//get the audio component from the player entity which has an ID of 1 
		//AudioComponent* audioComponent = reinterpret_cast<AudioComponent*>(this->entityManager->GetComponentOfEntity(1, AUDIO));
		//audioComponent->playSound("GunShot");
		PxExtendedVec3 extPos = controller->getFootPosition();
		PxVec3 pos = PxVec3(static_cast<float>(extPos.x), static_cast<float>(extPos.y), static_cast<float>(extPos.z));
		PxVec3 direction(0.f, 1.f, 1.f);
		direction.normalize();

		PxRaycastHit hit;
		PxRigidActor* selfActor = controller->getActor();
		IgnoreSelfFilterCallback filter(controller->getActor());
		bool hitflag = RaycastUtility::SingleHit(gScene, pos, direction, 100.0f, hit, &filter);
		if (hitflag) {
			if (hit.actor->is<PxRigidDynamic>()) {
				// hit dynamic
				DebugDrawRayInPVD(gScene, pos, pos + direction * hit.distance, 0xFF000000);
				std::cout << "Hit dynamic actor at (" << hit.position.x << ", " << hit.position.y << ", " << hit.position.z << ")\n";
			}
			else {
				// hit static
				std::cout << "HERE" << std::endl;
				DebugDrawRayInPVD(gScene, pos, pos + direction * hit.distance, 0xFFFFFF00);
				std::cout << "Hit static object\n";
			}
		}
		else {
			// hit nothing
			DebugDrawRayInPVD(gScene, pos, pos + direction * 100.0f, 0xFFFFFF00);
			std::cout << "Hit nothing\n";
		}
	}

	PxRaycastHit PhysicsWorld::handleShooting(Entity* playerEntity)
	{

		CameraComponent* cameraComponent = reinterpret_cast<CameraComponent*>(entityManager->GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
		glm::vec3 cameraPos = cameraComponent->GetCamera()->position;
		glm::vec3 cameraDir = cameraComponent->GetFrontDirection();
		PxExtendedVec3 extPos = PxExtendedVec3(cameraPos.x, cameraPos.y, cameraPos.z);
		PxVec3 direction(cameraDir.x, cameraDir.y, cameraDir.z);
		PxVec3 pos = PxVec3(static_cast<float>(extPos.x), static_cast<float>(extPos.y), static_cast<float>(extPos.z));
		direction.normalize();

		PxRaycastHit hit;
		PxRigidActor* selfActor = controller->getActor();
		IgnoreSelfFilterCallback filter(controller->getActor());
		bool hitflag = RaycastUtility::SingleHit(gScene, pos, direction, 1000.0f, hit, &filter);
		if (hitflag) {
			if (hit.actor->is<PxRigidDynamic>()) {
				// hit dynamic
				DebugDrawRayInPVD(gScene, pos, pos + direction * hit.distance, 0xFF000000);
				//std::cout << "Hit dynamic actor at (" << hit.position.x << ", " << hit.position.y << ", " << hit.position.z << ")\n";
			}
			else {
				// hit static
				DebugDrawRayInPVD(gScene, pos, pos + direction * hit.distance, 0xFFFFFF00);
				//hit.actor
				//std::cout << hit.actor->getName() << std::endl;
			}
		}
		else {
			// hit nothing
			DebugDrawRayInPVD(gScene, pos, pos + direction * 100.0f, 0xFFFFFF00);
			//std::cout << "Hit nothing\n";
		}
		AudioComponent* audioComponent = reinterpret_cast<AudioComponent*>(entityManager->GetComponentOfEntity(playerEntity->GetEntityId(), AUDIO));
		audioComponent->playSound("GunShot");

		return hit;
	}

	// update models matrices
	void PhysicsWorld::updateObjects(Engine::EntityManager& entityManager, std::vector<Engine::vk::Model>& models)
	{
		// get all PhysicsComponents which are simulated locally
		std::vector<ComponentBase*> physicsComponents = entityManager.GetSimulatedPhysicsComponents();
		for (std::size_t i = 0; i < physicsComponents.size(); i++) {
			PhysicsComponent* p = reinterpret_cast<PhysicsComponent*>(physicsComponents[i]);

			// dynamic update
			if (p->GetPhysicsType() == PhysicsComponent::PhysicsType::DYNAMIC)
			{
				//glm::mat4 matrix = ConvertPxTransformToGlmMat4(p->GetDynamicBody()->getGlobalPose());
				//matrix = glm::scale(matrix, p->GetScale());
				//entityManager.GetEntity(p->GetEntityId())->SetModelMatrix(matrix);
				//continue;
				Entity* entity = entityManager.GetEntity(p->GetEntityId());
				PxTransform transform = p->GetDynamicBody()->getGlobalPose();
				entity->SetPosition(transform.p.x, transform.p.y, transform.p.z);
				entity->SetRotation(glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z));
				entity->SetScale(p->GetScale().x, p->GetScale().y, p->GetScale().z);

				continue; //?
			}

			// controller update
			if (p->GetPhysicsType() == PhysicsComponent::PhysicsType::CONTROLLER)
			{
				//PxExtendedVec3 pos = p->GetController()->getFootPosition();
				//glm::vec3 glmPos = glm::vec3(pos.x, pos.y, pos.z);
				//glm::mat4 matrix = glm::translate(glm::mat4(1.0f), glmPos);
				//matrix = glm::scale(matrix, p->GetScale());

				//entityManager.GetEntity(p->GetEntityId())->SetModelMatrix(matrix);
				//Entity* entity = entityManager.GetEntity(p->GetEntityId());
				//PxTransform transform = p->GetDynamicBody()->getGlobalPose();
				//entity->SetPosition(transform.p.x, transform.p.y, transform.p.z);
				//entity->SetRotation(glm::quat(transform.q.w, transform.q.x, transform.q.y, transform.q.z));
				//entity->SetScale(p->GetScale().x, p->GetScale().y, p->GetScale().z);
				PxExtendedVec3 pos = p->GetController()->getFootPosition();
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

	void PhysicsWorld::DebugDrawRayInPVD(PxScene* scene, const PxVec3& start, const PxVec3& end, PxU32 color = 0xffffffff) {
		auto* client = scene->getScenePvdClient();
		if (!client) return;

		PxDebugLine line(start, end, color);

		client->drawLines(&line, 10);
		PxDebugPoint point(end, 0xff00ffff);
		client->drawPoints(&point, 1);
	}

}

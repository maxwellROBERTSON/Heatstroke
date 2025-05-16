#include "SinglePlayer.hpp"

#include "../../Engine/Core/Game.hpp"

#include "../../Engine/ECS/Components/AudioComponent.hpp"
#include "../../Engine/ECS/Components/CameraComponent.hpp"
#include "../../Engine/ECS/Components/NetworkComponent.hpp"
#include "../../Engine/ECS/Components/PhysicsComponent.hpp"
#include "../../Engine/ECS/Components/RenderComponent.hpp"
#include "../../Engine/ECS/Entity.hpp"
#include "../../Engine/ECS/EntityManager.hpp"

#include "../../Engine/Physics/PhysicsWorld.hpp"

#include "../../Engine/vulkan/VulkanContext.hpp"
#include "../../Engine/vulkan/VulkanDevice.hpp"

#include "../../Engine/Core/Camera.hpp"

#include "../DemoGame.hpp"

using namespace Engine;

std::random_device rd;  // Define random device
std::mt19937 gen(rd()); // Define mersenne_twister engine using rd as seed
std::uniform_int_distribution<> randomDistribX(-2.82f, 2.76f);
std::uniform_int_distribution<> randomDistribZ(-9.5f, 3.5f);

SinglePlayer::SinglePlayer(FPSTest* game) {
	this->game = game;
}

void SinglePlayer::Update(float timeDelta)
{
	fireDelay -= timeDelta;
	reloadDelay -= timeDelta;
	counter -= timeDelta;
	if (fireDelay <= 0.0f && !isReloading)
		canFire = true;

	if (reloadDelay <= 0.0f)
	{
		isReloading = false;
		canReload = true;
	}

	if (counter <= 0.0f && countdown > 0)
	{
		countdown--;
		counter = 1.0f;
	}

	Engine::EntityManager& entityManager = this->game->GetEntityManager();
	Engine::PhysicsWorld& physicsWorld = this->game->GetPhysicsWorld();
	Renderer& renderer = this->game->getRenderer();
	Engine::Camera* camera = renderer.getCameraPointer();

	// Update player camera or detached scene camera
	if (isPlayerCam)
	{
		physicsWorld.updatePhysics(timeDelta, true);
		camera->updateCamera(this->game->GetContext().getGLFWWindow(), timeDelta, false);
	}
	else
	{
		camera->updateCamera(this->game->GetContext().getGLFWWindow(), timeDelta, true);
	}

	if (playerEntity == nullptr || pistolEntity == nullptr || targetEntity == nullptr)
		return;

	if (InputManager::getKeyboard().isPressed(HS_KEY_P))

	{

		score = 0;
		countdown = 30;
		ammoCount = 10;
	}


	// Handle swap gun
	switch (InputManager::getInputDevice())
	{
	case InputDevice::CONTROLLER:
	{
		if (InputManager::getJoystick(0).isPressed(HS_GAMEPAD_BUTTON_Y))
			swapWeapon();

		if (InputManager::getJoystick(0).isPressed(HS_GAMEPAD_BUTTON_X))
			reloadPistol();

		if ((InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_TRIGGER) > -0.5f) && canFire)
			shootPistol();

		if ((InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_TRIGGER) > -0.5f) && !holdingPistol)
			shootRifle();

					// glm::vec3 pos = entity->GetPosition();
					// int xPos = randomDistribX(gen);
					// int zPos = randomDistribZ(gen);
					// glm::vec3 newPos{ xPos, pos.y, zPos };
					// entity->SetPosition(newPos);
					// glm::vec3 translation;
					// glm::vec3 scale;
					// glm::quat rotation;
					// physicsComponent->DecomposeTransform(entity->GetModelMatrix(), translation, rotation, scale);
					// PxTransform pxTransform(
					// 	PxVec3(translation.x, translation.y, translation.z),
					// 	PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
					// );
					// physicsComponent->GetStaticBody()->setGlobalPose(pxTransform);


		break;
	}
	case InputDevice::KBM:
	{
		//if (InputManager::Action(Controls::SwapWeapon))
		if (InputManager::getKeyboard().isPressed(HS_KEY_X))
			swapWeapon();

		//if (InputManager::Action(Controls::Reload))
		if (InputManager::getKeyboard().isPressed(HS_KEY_R))
			reloadPistol();

		//if (InputManager::Action(Controls::Shoot))
		//if (InputManager::getMouse().isDown(HS_MOUSE_BUTTON_LEFT) && canFire)
		if (InputManager::getMouse().isPressed(HS_MOUSE_BUTTON_LEFT) && canFire)
		{
			shootPistol();
		}

		if (InputManager::getMouse().isDown(HS_MOUSE_BUTTON_LEFT) && !holdingPistol)
			shootRifle();

		if (InputManager::getKeyboard().isPressed(HS_KEY_R) && !holdingPistol)
			reloadRifle();



		break;
	}
	}
	if (countdown <= 0)
		gameOver = true;
}

void SinglePlayer::ToggleSceneCamera(Camera* sceneCamera)
{
	Engine::EntityManager& entityManager = this->game->GetEntityManager();
	Renderer& renderer = this->game->getRenderer();

	Engine::CameraComponent* cameraComponent = reinterpret_cast<Engine::CameraComponent*>(this->game->GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
	Engine::RenderComponent* playerRenderComp = reinterpret_cast<Engine::RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), RENDER));

	if (renderer.getCameraPointer() == sceneCamera)
	{
		playerRenderComp->SetIsActive(false);
		renderer.attachCamera(cameraComponent->GetCamera());
		isPlayerCam = true;
	}
	else
	{
		playerRenderComp->SetIsActive(true);
		renderer.attachCamera(sceneCamera);
		isPlayerCam = false;
	}
}

void SinglePlayer::SetPlayerEntity(Engine::Entity* e)
{
	playerEntity = e;
	isPlayerCam = true;
	this->game->getRenderer().getCameraPointer()->init(this->game->GetContext().getGLFWWindow());
}

void SinglePlayer::swapWeapon()
{
	if (holdingPistol)
	{
		RenderComponent* pistolRenderComponent = reinterpret_cast<RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(pistolEntity->GetEntityId(), RENDER));
		pistolRenderComponent->SetIsActive(false);
		RenderComponent* rifleRenderComponent = reinterpret_cast<RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(rifleEntity->GetEntityId(), RENDER));
		rifleRenderComponent->SetIsActive(true);
		holdingPistol = false;
	}
	else
	{
		RenderComponent* pistolRenderComponent = reinterpret_cast<RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(pistolEntity->GetEntityId(), RENDER));
		pistolRenderComponent->SetIsActive(true);
		RenderComponent* rifleRenderComponent = reinterpret_cast<RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(rifleEntity->GetEntityId(), RENDER));
		rifleRenderComponent->SetIsActive(false);
		holdingPistol = true;
	}
}

void SinglePlayer::reloadPistol()
{
	if (canReload)
	{
		canReload = false;
		isReloading = true;
		reloadDelay = 0.5f;
		RenderComponent* pistolRenderComponent = reinterpret_cast<RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(pistolEntity->GetEntityId(), RENDER));
		std::vector<vk::Model>& models = this->game->GetModels();
		int pistolModelIndex = pistolRenderComponent->GetModelIndex();
		models[pistolModelIndex].animationQueue.push(models[pistolModelIndex].animations[4]);
		models[pistolModelIndex].blending = true;
		ammoCount = 10;
	}
}

void SinglePlayer::shootPistol()
{
	Engine::RenderComponent* pistolRenderComponent = reinterpret_cast<Engine::RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(pistolEntity->GetEntityId(), RENDER));
	Engine::AudioComponent* playerAudioComponent = reinterpret_cast<Engine::AudioComponent*>(this->game->GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), AUDIO));
	int pistolModelIndex = pistolRenderComponent->GetModelIndex();
	std::vector<vk::Model>& models = this->game->GetModels();

	if (ammoCount <= 0)
	{
		reloadPistol();
	}
	else
	{
		if (models[pistolModelIndex].animationQueue.empty())
		{
			models[pistolModelIndex].animationQueue.push(models[pistolModelIndex].animations[3]);
			models[pistolModelIndex].blending = true;
			ammoCount--;
			canFire = false;
			fireDelay = 0.25f;

			PxRaycastHit entityHit = this->game->GetPhysicsWorld().handleShooting();

			bool hitTarget = false;

			std::vector<int> entitiesWithPhysicsComponent = this->game->GetEntityManager().GetEntitiesWithComponent(PHYSICS);
			for (int i = 0; i < entitiesWithPhysicsComponent.size(); i++)
			{
				Engine::Entity* entity = this->game->GetEntityManager().GetEntity(entitiesWithPhysicsComponent[i]);
				Engine::PhysicsComponent* physicsComponent = reinterpret_cast<Engine::PhysicsComponent*>(this->game->GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
				if (physicsComponent->GetStaticBody() != nullptr && physicsComponent->GetStaticBody() == entityHit.actor)
				{
					if (countdown > 0)
						score++;

					Engine::RenderComponent* hitRenderComponent = reinterpret_cast<Engine::RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
					hitRenderComponent->SetIsActive(0);
					glm::vec3 pos = entity->GetPosition();
					int xPos = randomDistribX(gen);
					int zPos = randomDistribZ(gen);
					glm::vec3 newPos{ xPos, pos.y, zPos };
					targetEntity->SetPosition(newPos);
					glm::vec3 translation;
					glm::vec3 scale;
					glm::quat rotation;
					physicsComponent->DecomposeTransform(targetEntity->GetModelMatrix(), translation, rotation, scale);
					PxTransform pxTransform(
						PxVec3(translation.x, translation.y, translation.z),
						PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
					);
					physicsComponent->GetStaticBody()->setGlobalPose(pxTransform);
					hitRenderComponent->SetIsActive(1);

					hitTarget = true;
				}
			}

			if (!hitTarget) {
				if (entityHit.actor != nullptr && entityHit.actor->getName() != "levelBounds" && entityHit.distance != PX_MAX_REAL) {
					this->game->getBulletDecals().setNextDecal(entityHit.position, entityHit.normal);
				}
			}
		}
	}
}

void SinglePlayer::reloadRifle()
{
	if (canReload)
	{
		//canReload = false;
		//isReloading = true;
		//reloadDelay = 0.5f;
		RenderComponent* rifleRenderComponent = reinterpret_cast<RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(rifleEntity->GetEntityId(), RENDER));
		std::vector<vk::Model>& models = this->game->GetModels();
		int rifleModelIndex = rifleRenderComponent->GetModelIndex();
		models[rifleModelIndex].animationQueue.push(models[rifleModelIndex].animations[4]);
		models[rifleModelIndex].blending = true;
		//ammoCount = 6;
	}
}

void SinglePlayer::shootRifle()
{
	Engine::RenderComponent* rifleRenderComponent = reinterpret_cast<Engine::RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(rifleEntity->GetEntityId(), RENDER));
	Engine::AudioComponent* playerAudioComponent = reinterpret_cast<Engine::AudioComponent*>(this->game->GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), AUDIO));
	int rifleModelIndex = rifleRenderComponent->GetModelIndex();
	std::vector<vk::Model>& models = this->game->GetModels();

	if (ammoCount <= 0)
	{
		reloadRifle();
	}
	else
	{
		if (models[rifleModelIndex].animationQueue.empty())
		{
			models[rifleModelIndex].animationQueue.push(models[rifleModelIndex].animations[6]);
			models[rifleModelIndex].blending = true;
			ammoCount--;
			canFire = false;
			fireDelay = 0.01f;
			//fireDelay = 1.0f;

			PxRaycastHit entityHit = this->game->GetPhysicsWorld().handleShooting();
			bool hitTarget = false;

			std::vector<int> entitiesWithPhysicsComponent = this->game->GetEntityManager().GetEntitiesWithComponent(PHYSICS);
			for (int i = 0; i < entitiesWithPhysicsComponent.size(); i++)
			{
				Engine::Entity* entity = this->game->GetEntityManager().GetEntity(entitiesWithPhysicsComponent[i]);
				Engine::PhysicsComponent* physicsComponent = reinterpret_cast<Engine::PhysicsComponent*>(this->game->GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
				if (physicsComponent->GetStaticBody() != nullptr && physicsComponent->GetStaticBody() == entityHit.actor)
				{
					score++;

					Engine::RenderComponent* hitRenderComponent = reinterpret_cast<Engine::RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
					hitRenderComponent->SetIsActive(0);
					glm::vec3 pos = entity->GetPosition();
					int xPos = randomDistribX(gen);
					int zPos = randomDistribZ(gen);
					glm::vec3 newPos{ xPos, pos.y, zPos };
					targetEntity->SetPosition(newPos);
					glm::vec3 translation;
					glm::vec3 scale;
					glm::quat rotation;
					physicsComponent->DecomposeTransform(targetEntity->GetModelMatrix(), translation, rotation, scale);
					PxTransform pxTransform(
						PxVec3(translation.x, translation.y, translation.z),
						PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
					);
					physicsComponent->GetStaticBody()->setGlobalPose(pxTransform);
					hitRenderComponent->SetIsActive(1);

					hitTarget = true;
				}
			}

			if (!hitTarget) {
				if (entityHit.actor != nullptr && entityHit.actor->getName() != "levelBounds" && entityHit.distance != PX_MAX_REAL) {
					this->game->getBulletDecals().setNextDecal(entityHit.position, entityHit.normal);
				}
			}
		}
	}
}

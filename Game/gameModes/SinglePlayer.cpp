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

#include "../../Engine/vulkan/Renderer.hpp"
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
	counter -= timeDelta;
	if (fireDelay <= 0.0f)
		canFire = true;

	if (counter <= 0.0f && countdown > 0)
	{
		countdown--;
		counter = 1.0f;
	}

	Engine::EntityManager& entityManager = this->game->GetEntityManager();
	Engine::PhysicsWorld& physicsWorld = this->game->GetPhysicsWorld();
	Engine::Renderer& renderer = this->game->GetRenderer();
	Engine::Camera* camera = renderer.GetCameraPointer();

	// Update player camera or detached scene camera
	if (isPlayerCam)
	{
		physicsWorld.updatePhysics(timeDelta, true);
		renderer.GetCameraPointer()->updateCamera(this->game->GetContext().getGLFWWindow(), timeDelta, false);
	}
	else
	{
		renderer.GetCameraPointer()->updateCamera(this->game->GetContext().getGLFWWindow(), timeDelta, true);
	}

	if (playerEntity == nullptr || pistolEntity == nullptr || targetEntity == nullptr)
		return;

	// Handle shooting
	if (Engine::InputManager::getMouse().isPressed(HS_MOUSE_BUTTON_LEFT) && canFire)
	{
		Engine::RenderComponent* pistolRenderComponent = reinterpret_cast<Engine::RenderComponent*>(entityManager.GetComponentOfEntity(pistolEntity->GetEntityId(), RENDER));
		Engine::AudioComponent* playerAudioComponent = reinterpret_cast<Engine::AudioComponent*>(entityManager.GetComponentOfEntity(playerEntity->GetEntityId(), AUDIO));
		int pistolModelIndex = pistolRenderComponent->GetModelIndex();
		std::vector<vk::Model>& models = this->game->GetModels();
		models[pistolModelIndex].animationQueue.push(models[pistolModelIndex].animations[3]);
		models[pistolModelIndex].blending = true;
		canFire = false;
		fireDelay = 1.0f;
		PxRaycastHit entityHit = physicsWorld.handleShooting();

		bool hitTarget = false;

		std::vector<int> entitiesWithPhysicsComponent = entityManager.GetEntitiesWithComponent(PHYSICS);
		for (int i = 0; i < entitiesWithPhysicsComponent.size(); i++)
		{
			Engine::Entity* entity = entityManager.GetEntity(entitiesWithPhysicsComponent[i]);
			Engine::PhysicsComponent* physicsComponent = reinterpret_cast<Engine::PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
			if (physicsComponent->GetStaticBody() != nullptr && physicsComponent->GetStaticBody() == entityHit.actor)
			{
				score++;

				Engine::RenderComponent* hitRenderComponent = reinterpret_cast<Engine::RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
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
			if (entityHit.actor->getName() != "levelBounds" && entityHit.distance != PX_MAX_REAL) {
				this->game->getDecals().setNextDecal(entityHit.position, entityHit.normal);
			}
		}

		if (countdown <= 0)
			gameOver = true;
	}
}

void SinglePlayer::ToggleSceneCamera(Camera* sceneCamera)
{
	Engine::EntityManager& entityManager = this->game->GetEntityManager();
	Engine::Renderer& renderer = this->game->GetRenderer();

	Engine::CameraComponent* cameraComponent = reinterpret_cast<Engine::CameraComponent*>(this->game->GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
	Engine::RenderComponent* playerRenderComp = reinterpret_cast<Engine::RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), RENDER));

	if (renderer.GetCameraPointer() == sceneCamera)
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
	this->game->GetRenderer().GetCameraPointer()->init(this->game->GetContext().getGLFWWindow());
}
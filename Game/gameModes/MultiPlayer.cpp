#include "MultiPlayer.hpp"

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

MultiPlayer::MultiPlayer(FPSTest* game) {
	this->game = game;
}

void MultiPlayer::InitNetwork()
{
	EntityManager* manager = &(game->GetEntityManager());
	GameClient* client = reinterpret_cast<GameClient*>(game->GetNetwork().GetNetworkTypePointer());

	std::vector<int> entitiesWithNetworkComponent = manager->GetEntitiesWithComponent(NETWORK);
	int thisClientEntity = -1;
	NetworkComponent* networkComponent;
	for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
	{
		networkComponent = reinterpret_cast<NetworkComponent*>(manager->GetComponentOfEntity(entitiesWithNetworkComponent[i], NETWORK));
		if (networkComponent->GetClientId() == client->GetClientId())
		{
			CameraComponent* cameraComponent = reinterpret_cast<CameraComponent*>(manager->GetComponentOfEntity(entitiesWithNetworkComponent[i], CAMERA));
			game->getRenderer().attachCamera(cameraComponent->GetCamera());
			client->SetClientEntityId(entitiesWithNetworkComponent[i]);
			Entity* thisEntity = manager->GetEntity(entitiesWithNetworkComponent[i]);
			SetPlayerEntity(thisEntity);
			ChildrenComponent* childrenComponent = reinterpret_cast<ChildrenComponent*>(manager->GetComponentOfEntity(entitiesWithNetworkComponent[i], CHILDREN));
			SetPistolEntity(manager->GetEntity(childrenComponent->GetChildrenEntityIds()[0]));
			thisClientEntity = entitiesWithNetworkComponent[i];
			break;
		}
	}

	std::vector<int> vec = manager->GetEntitiesWithComponent(PHYSICS);
	PhysicsComponent* physicsComp;
	RenderComponent* renderComp;

	manager->ClearUpdatedPhysicsComps();

	for (int i = 0; i < vec.size(); i++)
	{
		physicsComp = reinterpret_cast<PhysicsComponent*>(manager->GetComponentOfEntity(vec[i], PHYSICS));
		renderComp = reinterpret_cast<RenderComponent*>(manager->GetComponentOfEntity(vec[i], RENDER));
		glm::mat4 mat = manager->GetEntity(vec[i])->GetModelMatrix();
		PhysicsComponent::PhysicsType type = physicsComp->GetPhysicsType();
		Engine::vk::Model& model = game->GetModels()[renderComp->GetModelIndex()];
		if (type == PhysicsComponent::PhysicsType::STATIC || type == PhysicsComponent::PhysicsType::STATICBOUNDED)
		{
			physicsComp->InitComplexShape("Temp", game->GetPhysicsWorld(), type, model, mat, vec[i]);
		}
		else
		{
			if (vec[i] == thisClientEntity)
			{
				physicsComp->Init(game->GetPhysicsWorld(), type, model, mat, vec[i], true, true);
			}
			else
			{
				physicsComp->Init(game->GetPhysicsWorld(), type, model, mat, vec[i], true, false);
				manager->AddUpdatedPhysicsComp(physicsComp, renderComp->GetIsActive());
			}
		}
	}

	game->GetEntityManager().ResetChanged();
	client->ReadyToSendInitMessage();
}

void MultiPlayer::Update(float timeDelta)
{
	Engine::EntityManager& entityManager = this->game->GetEntityManager();
	Engine::PhysicsWorld& physicsWorld = this->game->GetPhysicsWorld();
	Renderer& renderer = this->game->getRenderer();

	fireDelay -= timeDelta;
	reloadDelay -= timeDelta;

	if (fireDelay <= 0.0f && !isReloading)
		canFire = true;

	counter -= timeDelta;


	if (counter <= 0.0f)
	{
		counter = 1.0f;
	}

	if (entityManager->GetResetTimer != 0)
		return;
	physicsWorld.updatePhysics(timeDelta, true);
	renderer.getCameraPointer()->updateCamera(this->game->GetContext().getGLFWWindow(), timeDelta, false);

	if (playerEntity == nullptr || pistolEntity == nullptr)
		return;

	// Handle shooting

	if (InputManager::Action(Controls::Reload))
	{
		//RenderComponent* pistolRenderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(pistolEntity->GetEntityId(), RENDER));
		//std::vector<vk::Model>& models = this->game->GetModels();
		//int pistolModelIndex = pistolRenderComponent->GetModelIndex();
		//models[pistolModelIndex].animationQueue.push(models[pistolModelIndex].animations[4]);
		//models[pistolModelIndex].blending = true;
		//pistolAmmoCount = 6;
		//models[pistolModelIndex].animationQueue.pop();
		reloadPistol();

	}

	if (InputManager::getMouse().isPressed(HS_MOUSE_BUTTON_LEFT) && canFire)
	{
		shootPistol();
	}
}

void MultiPlayer::SetPlayerEntity(Engine::Entity* e)
{
	playerEntity = e;
	this->game->getRenderer().getCameraPointer()->init(this->game->GetContext().getGLFWWindow());
}

glm::vec3 MultiPlayer::GetStartPos(int team)
{
	if (team > 0 && team <= 4)
		return startPositions[team - 1];
	else
		throw("Team < 1 or Team > 4");
}

void MultiPlayer::reloadPistol()
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
		pistolAmmoCount = 10;
	}
}

void MultiPlayer::shootPistol()
{
	Engine::RenderComponent* pistolRenderComponent = reinterpret_cast<Engine::RenderComponent*>(this->game->GetEntityManager().GetComponentOfEntity(pistolEntity->GetEntityId(), RENDER));
	Engine::AudioComponent* playerAudioComponent = reinterpret_cast<Engine::AudioComponent*>(this->game->GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), AUDIO));
	int pistolModelIndex = pistolRenderComponent->GetModelIndex();
	std::vector<vk::Model>& models = this->game->GetModels();

	if (pistolAmmoCount <= 0)
	{
		reloadPistol();
	}
	else
	{
		if (models[pistolModelIndex].animationQueue.empty())
		{

			//		std::vector<int> entitiesWithPhysicsComponent = entityManager.GetEntitiesWithComponent(PHYSICS);
			//		for (int i = 0; i < entitiesWithPhysicsComponent.size(); i++)
			//		{
			//			Engine::Entity* entity = entityManager.GetEntity(entitiesWithPhysicsComponent[i]);
			//			Engine::PhysicsComponent* physicsComponent = reinterpret_cast<Engine::PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
			//			if (physicsComponent->GetComponentActor() != nullptr && physicsComponent->GetComponentActor() == entityHit.actor)
			//			{
			//				score++;
			//				hit = true;
			//			}
			//		}

			//		if (hit && entityManager.GetResetTimerInt() == 0)
			//		{
			//			game->GetNetwork().GetNetworkTypePointer()->ReadyToSendResetMessage();
			//		}

			//		else {
			//			if (entityHit.actor != nullptr && entityHit.actor->getName() != "levelBounds" && entityHit.distance != PX_MAX_REAL) {
			//				this->game->getBulletDecals().setNextDecal(entityHit.position, entityHit.normal);
			//			}
			//		}
			models[pistolModelIndex].animationQueue.push(models[pistolModelIndex].animations[3]);
			models[pistolModelIndex].blending = true;
			pistolAmmoCount--;
			canFire = false;
			fireDelay = 0.25f;

			PxRaycastHit entityHit = this->game->GetPhysicsWorld().handleShooting();
			bool hit = false;

			std::vector<int> entitiesWithPhysicsComponent = this->game->GetEntityManager().GetEntitiesWithComponent(PHYSICS);
			for (int i = 0; i < entitiesWithPhysicsComponent.size(); i++)
			{

				Engine::Entity* entity = this->game->GetEntityManager().GetEntity(entitiesWithPhysicsComponent[i]);
				Engine::PhysicsComponent* physicsComponent = reinterpret_cast<Engine::PhysicsComponent*>(this->game->GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
				if (physicsComponent->GetComponentActor() != nullptr && physicsComponent->GetComponentActor() == entityHit.actor)
				{
					score++;
					hit = true;
				}
			}

			if (hit && this->game->GetEntityManager().GetResetTimerInt() == 0)
			{
				game->GetNetwork().GetNetworkTypePointer()->ReadyToSendResetMessage();
			}

			else {
				if (entityHit.actor != nullptr && entityHit.actor->getName() != "levelBounds" && entityHit.distance != PX_MAX_REAL) {
					this->game->getBulletDecals().setNextDecal(entityHit.position, entityHit.normal);
				}
			}
		}
	}
}

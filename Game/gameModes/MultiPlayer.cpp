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
				Entity* entity = manager->GetEntity(vec[i]);

				mat = manager->GetEntity(vec[i])->GetModelMatrix();
				physicsComp->Init(game->GetPhysicsWorld(), type, model, mat, vec[i], true, true);
				manager->AddSimulatedPhysicsEntity(vec[i]);
				/*glm::vec3 translation;
				glm::quat rotation;
				glm::vec3 scale;
				Entity* entity;
				if (physicsComp->DecomposeTransform(mat, translation, rotation, scale))
				{
					entity = manager->GetEntity(vec[i]);
					entity->SetPosition(translation);
					entity->SetRotation(rotation);
					entity->SetScale(scale.x, scale.y, scale.z);
				}
				else
				{
					throw ("Failed to decompose matrix for client's entity");
				}*/
			}
			else
			{
				physicsComp->Init(game->GetPhysicsWorld(), type, model, mat, vec[i], true, false);
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
	counter -= timeDelta;
	if (fireDelay <= 0.0f)
		canFire = true;

	if (counter <= 0.0f && countdown > 0)
	{
		countdown--;
		counter = 1.0f;
	}

	/*if (countdown <= 0)
	{
		int score = 0;
		int countdown = 60;
		float fireDelay = 1.5f;
		bool canFire = true;
		float counter = 1.0f;
		int ammoCount = 6;
		std::vector<int> entitiesWithPhysicsComponent = entityManager.GetEntitiesWithComponent(PHYSICS);
		for (int i = 0; i < entitiesWithPhysicsComponent.size(); i++)
		{
			Engine::Entity* entity = entityManager.GetEntity(entitiesWithPhysicsComponent[i]);
			Engine::PhysicsComponent* physicsComponent = reinterpret_cast<Engine::PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
			if (physicsComponent->GetStaticBody() != nullptr && physicsComponent->GetStaticBody() == entityHit.actor)
			{
				score++;

				Engine::NetworkComponent* networkComponent = reinterpret_cast<Engine::NetworkComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), NETWORK));
				entity->SetPosition(GetStartPos(networkComponent->GetTeam()));
				glm::vec3 translation;
				glm::vec3 scale;
				glm::quat rotation;
				physicsComponent->DecomposeTransform(entity->GetModelMatrix(), translation, rotation, scale);
				PxTransform pxTransform(
					PxVec3(translation.x, translation.y, translation.z),
					PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
				);
				physicsComponent->GetStaticBody()->setGlobalPose(pxTransform);

				hit = true;
			}
		}
	}*/

	physicsWorld.updatePhysics(timeDelta, true);
	renderer.getCameraPointer()->updateCamera(this->game->GetContext().getGLFWWindow(), timeDelta, false);

	if (playerEntity == nullptr || pistolEntity == nullptr)
		return;

	// Handle shooting

	if (InputManager::Action(Controls::Reload))
	{
		RenderComponent* pistolRenderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(pistolEntity->GetEntityId(), RENDER));
		std::vector<vk::Model>& models = this->game->GetModels();
		int pistolModelIndex = pistolRenderComponent->GetModelIndex();
		models[pistolModelIndex].animationQueue.push(models[pistolModelIndex].animations[4]);
		models[pistolModelIndex].blending = true;
		ammoCount = 6;
		models[pistolModelIndex].animationQueue.pop();
	}

	if (InputManager::Action(Controls::Shoot) && canFire)
	{
		Engine::RenderComponent* pistolRenderComponent = reinterpret_cast<Engine::RenderComponent*>(entityManager.GetComponentOfEntity(pistolEntity->GetEntityId(), RENDER));
		Engine::AudioComponent* playerAudioComponent = reinterpret_cast<Engine::AudioComponent*>(entityManager.GetComponentOfEntity(playerEntity->GetEntityId(), AUDIO));
		int pistolModelIndex = pistolRenderComponent->GetModelIndex();
		std::vector<vk::Model>& models = this->game->GetModels();

		if (ammoCount <= 0)
		{
			models[pistolModelIndex].animationQueue.push(models[pistolModelIndex].animations[4]);
			models[pistolModelIndex].blending = true;
			ammoCount = 7;
		}
		else
		{
			models[pistolModelIndex].animationQueue.push(models[pistolModelIndex].animations[3]);
			models[pistolModelIndex].blending = true;
			ammoCount--;
			canFire = false;
			fireDelay = 1.5f;

			PxRaycastHit entityHit = physicsWorld.handleShooting();
			bool hit = false;

			std::vector<int> entitiesWithPhysicsComponent = entityManager.GetEntitiesWithComponent(PHYSICS);
			for (int i = 0; i < entitiesWithPhysicsComponent.size(); i++)
			{
				Engine::Entity* entity = entityManager.GetEntity(entitiesWithPhysicsComponent[i]);
				Engine::PhysicsComponent* physicsComponent = reinterpret_cast<Engine::PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
				if (physicsComponent->GetStaticBody() != nullptr && physicsComponent->GetStaticBody() == entityHit.actor)
				{
					score++;

					Engine::NetworkComponent* networkComponent = reinterpret_cast<Engine::NetworkComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), NETWORK));
					entity->SetPosition(GetStartPos(networkComponent->GetTeam()));
					glm::vec3 translation;
					glm::vec3 scale;
					glm::quat rotation;
					physicsComponent->DecomposeTransform(entity->GetModelMatrix(), translation, rotation, scale);
					PxTransform pxTransform(
						PxVec3(translation.x, translation.y, translation.z),
						PxQuat(rotation.x, rotation.y, rotation.z, rotation.w)
					);
					physicsComponent->GetStaticBody()->setGlobalPose(pxTransform);

					hit = true;
				}
			}

			if (!hit) {
				if (entityHit.actor != nullptr && entityHit.actor->getName() != "levelBounds" && entityHit.distance != PX_MAX_REAL) {
					this->game->getDecals().setNextDecal(entityHit.position, entityHit.normal);
				}
			}
		}
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
		return startPositions[team];
	else
		throw("Team < 1 or Team > 4");
}
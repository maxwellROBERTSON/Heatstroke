#include "SinglePlayer.hpp"

using namespace Engine;

std::random_device rd;  // Define random device
std::mt19937 gen(rd()); // Define mersenne_twister engine using rd as seed
std::uniform_int_distribution<> randomDistrib(1, 7); // Define the distribution

void SinglePlayer::Update(Game* game, float timeDelta)
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

	EntityManager& entityManager = game->GetEntityManager();
	PhysicsWorld& physicsWorld = game->GetPhysicsWorld();
	Renderer& renderer = game->GetRenderer();
	Camera* camera = renderer.GetCameraPointer();

	// Update player camera or detached scene camera
	if (isPlayerCam)
	{
		physicsWorld.updatePhysics(timeDelta, true);
		renderer.GetCameraPointer()->updateCamera(game->GetContext().getGLFWWindow(), timeDelta, false);
	}
	else
	{
		renderer.GetCameraPointer()->updateCamera(game->GetContext().getGLFWWindow(), timeDelta, true);
	}

	if (playerEntity == nullptr || pistolEntity == nullptr || targetEntity == nullptr)
		return;

	// Handle shooting
	if (Engine::InputManager::getMouse().isPressed(HS_MOUSE_BUTTON_LEFT) && canFire)
	{
		RenderComponent* pistolRenderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(pistolEntity->GetEntityId(), RENDER));
		AudioComponent* playerAudioComponent = reinterpret_cast<AudioComponent*>(entityManager.GetComponentOfEntity(playerEntity->GetEntityId(), AUDIO));
		int pistolModelIndex = pistolRenderComponent->GetModelIndex();
		std::vector<vk::Model>& models = game->GetModels();
		models[pistolModelIndex].animationQueue.push(models[pistolModelIndex].animations[3]);
		models[pistolModelIndex].blending = true;
		canFire = false;
		fireDelay = 1.0f;
		PxRaycastHit entityHit = physicsWorld.handleShooting();
		std::vector<int> entitiesWithPhysicsComponent = entityManager.GetEntitiesWithComponent(PHYSICS);
		for (int i = 0; i < entitiesWithPhysicsComponent.size(); i++)
		{
			Entity* entity = entityManager.GetEntity(entitiesWithPhysicsComponent[i]);
			PhysicsComponent* physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
			if (physicsComponent->GetStaticBody() != nullptr && physicsComponent->GetStaticBody() == entityHit.actor)
			{
				score++;

				RenderComponent* hitRenderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
				hitRenderComponent->SetIsActive(0);
				int xPos = randomDistrib(gen);
				int yPos = randomDistrib(gen);
				glm::vec3 newPos{ xPos, yPos, 0.0f };
				targetEntity->SetPosition(newPos);
				PxTransform pxTransform(
					PxVec3(newPos.x, newPos.y, newPos.z)
				);
				//physicsComponent->SetTranslation(newPos);
				physicsComponent->GetStaticBody()->setGlobalPose(pxTransform);
				hitRenderComponent->SetIsActive(1);
			}
		}

		if (countdown <= 0)
			gameOver = true;
	}
}

void SinglePlayer::ToggleSceneCamera(Game* game, Camera* sceneCamera)
{
	EntityManager& entityManager = game->GetEntityManager();
	Renderer& renderer = game->GetRenderer();

	CameraComponent* cameraComponent = reinterpret_cast<CameraComponent*>(game->GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));

	if (renderer.GetCameraPointer() == sceneCamera)
	{
		renderer.attachCamera(cameraComponent->GetCamera());
		isPlayerCam = true;
	}
	else
	{
		renderer.attachCamera(sceneCamera);
		isPlayerCam = false;
	}
}

void SinglePlayer::SetPlayerEntity(Game* g, Entity* e)
{
	playerEntity = e;
	isPlayerCam = true;
	g->GetRenderer().GetCameraPointer()->init(g->GetContext().getGLFWWindow());
}
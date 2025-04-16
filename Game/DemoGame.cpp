#include "DemoGame.hpp"

#include <algorithm>
#include <chrono>
#include <future>
#include <thread>

#include "../Engine/vulkan/objects/Buffer.hpp"
#include "../Engine/vulkan/PipelineCreation.hpp"
#include "../Engine/vulkan/Renderer.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"

#include "Error.hpp"
#include "glm/gtx/string_cast.hpp"
#include "toString.hpp"

#include <imgui.h>

using namespace Engine;

float fireDelay = 1.0f;
//bool canFire = true;
bool canFire = false;

void FPSTest::Init()
{
	//create thread which then begins execution of initialiseModels
	std::thread initialiseModelsThread(&FPSTest::initialiseModels, this);

	std::cout << "Waiting for the execution of modelsThread to finish..." << std::endl;

	//blocks execution of the rest of the program until the initialiseModelsThread has finished
	initialiseModelsThread.join();

	// Cameras
	sceneCam = Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));

	playerEntity = 0;
	mapEntity = 0;
	playerCam = FPSCamera("Player Cam", 100.0f, 0.01f, 256.0f, playerPos, glm::vec3(0.0f, 0.0f, 1.0f));

	GetPhysicsWorld().init();
	GetRenderer().initialiseRenderer();
	GetGUI().initGUI();
	GetRenderer().attachCamera(&playerCam);
	//GetRenderer().attachCamera(&sceneCam);
	GetRenderer().initialiseModelDescriptors(GetModels());
}

void FPSTest::Render()
{
	// previous = std::chrono::steady_clock::now();
	while (!glfwWindowShouldClose(this->GetContext().getGLFWWindow()))
	{
		Update();
	}
	GetRenderer().finishRendering();
}

void FPSTest::Update() {

	Engine::Renderer& renderer = GetRenderer();
	Engine::PhysicsWorld& physicsWorld = GetPhysicsWorld();
	Engine::GUI& gui = GetGUI();

	Engine::InputManager::Update();
	GetNetwork().Update();

	if (renderer.checkSwapchain())
		return;

	if (renderer.acquireSwapchainImage())
		return;

	// Calculate time delta
	const auto now = std::chrono::steady_clock::now();
	const auto timeDelta = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - previous).count();
	previous = now;

	renderer.GetCameraPointer()->updateCamera(this->GetContext().getGLFWWindow(), timeDelta);

	switch (camMode)
	{
	case(CameraMode::SCENE):
		GetRenderer().attachCamera(&sceneCam);
		break;
	case(CameraMode::PLAYER):
		GetRenderer().attachCamera(&playerCam);
		break;
	default:
		GetRenderer().attachCamera(&sceneCam);
		break;
	}

	float fixedTimeDelta = std::min<float>(0.016f, timeDelta);

	//fireDelay -= fixedTimeDelta;
	//if (fireDelay <= 0.0f)
	//	canFire = true;

	// update PVD
	physicsWorld.gScene->simulate(fixedTimeDelta);
	physicsWorld.gScene->fetchResults(true);
	// update physics
	physicsWorld.updateObjects(GetEntityManager(), GetModels());

	if (camMode == CameraMode::PLAYER && playerEntity != nullptr)
	{
		physicsWorld.updateCharacter(playerEntity, fixedTimeDelta);

		PhysicsComponent* physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), PHYSICS));
		FPSCameraComponent* cameraComponent = reinterpret_cast<FPSCameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
		cameraComponent->SetCameraOffset(cameraOffset);
		//std::cout << glm::to_string(cameraPos) << std::endl;

		playerEntity->frontDirection = cameraComponent->GetFrontDirection();
		playerEntity->frontDirection.y = 0.0f;
		//playerPos = playerEntity->GetPosition();
		////playerPos -= glm::vec3(0.0f, 0.0f, 0.5f);
		//playerPos -= glm::vec3(0.0f, 1.0f, 0.0f);
		//playerEntity->SetPosition(playerPos);
		glm::mat4 playerRotation = glm::mat4(1.0f);
		playerRotation = glm::rotate(playerRotation, glm::radians(-cameraComponent->GetCamera()->yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		playerRotation = glm::rotate(playerRotation, glm::radians(-cameraComponent->GetCamera()->pitch), glm::vec3(1.0f, 0.0f, 0.0f));
		playerEntity->SetRotation(playerRotation);
		//playerPos = playerEntity->GetPosition();
		//playerPos += glm::vec3(0.0f, 0.0f, 0.5f);
		//playerPos += glm::vec3(0.0f, 1.0f, 0.0f);
		//playerEntity->SetPosition(playerPos);
		glm::vec3 cameraPos = playerEntity->GetPosition();
		cameraComponent->UpdateCameraPosition(cameraPos);


		if (Engine::InputManager::getMouse().isPressed(HS_MOUSE_BUTTON_LEFT))
		{
			if (canFire)
			{
				RenderComponent* playerRenderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), RENDER));
				int playerModelIndex = playerRenderComponent->GetModelIndex();
				std::vector<vk::Model>& models = GetModels();
				models[playerModelIndex].animationQueue.push(models[playerModelIndex].animations[3]);
				models[playerModelIndex].blending = true;
				canFire = false;
				fireDelay = 1.0f;
			}
		}
	}



	renderer.updateAnimations(timeDelta);
	renderer.updateUniforms();
	gui.makeGUI();
	renderer.render(GetModels());
	renderer.submitRender();
}


void FPSTest::OnEvent(Engine::Event& e)
{
	Game::OnEvent(e);
	GetRenderer().GetCameraPointer()->OnEvent(this->GetContext().getGLFWWindow(), e);
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<KeyPressedEvent>([&](KeyPressedEvent& event)
		{
			if (event.GetKeyCode() == HS_KEY_C)
			{
				switch (camMode)
				{
				case FPSTest::SCENE:
					camMode = FPSTest::PLAYER;

					break;
				case FPSTest::PLAYER:
					camMode = FPSTest::SCENE;
					break;
				default:
					camMode = FPSTest::SCENE;
					break;
				}
			}
			return true;
		});
}

void FPSTest::DrawGUI()
{
	FPSCameraComponent* cameraComponent = reinterpret_cast<FPSCameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));

	ImGui::Begin("Game:");
	ImGui::Text("GAME DEBUG");
	ImGui::Text("Player Rotations:");
	ImGui::SliderFloat("Cam: Pitch", &cameraComponent->GetCamera()->pitch, -60.0f, 60.0f);
	ImGui::SliderFloat("Cam: Yaw", &cameraComponent->GetCamera()->yaw, -360.0f, 360.0f);
	ImGui::InputFloat3("Cam Offset: ", &cameraOffset.x);
	//ImGui::Text("Yaw: %f, Pitch: %f", cameraComponent->GetCamera()->yaw, cameraComponent->GetCamera()->pitch);
	ImGui::Text("Game Settings:");
	ImGui::InputFloat("Fire Delay", &fireDelay);
	if (ImGui::Checkbox("Cam Can Rotate", &camCanRotate))
	{
		cameraComponent->setRotationEnabled(camCanRotate);
		int winX, winY;
		glfwGetFramebufferSize(this->GetContext().getGLFWWindow(), &winX, &winY);
		cameraComponent->GetCamera()->lastX = (float)winX / 2;
		cameraComponent->GetCamera()->lastY = (float)winY / 2;
		cameraComponent->GetCamera()->yaw = 0.0f;
		cameraComponent->GetCamera()->pitch = 0.0f;
	};
	ImGui::End();
}

void FPSTest::initialiseModels()
{
	// Here we would load all relevant glTF models and put them in the models vector
	tinygltf::Model sponza = Engine::loadFromFile("Game/assets/Sponza/glTF/Sponza.gltf");
	tinygltf::Model pistol = Engine::loadFromFile("Game/assets/Assets/guns/rifle/scene.gltf");
	//tinygltf::Model pistol = Engine::loadFromFile("Game/assets/Assets/guns/pistol1/scene.gltf");
	//tinygltf::Model pistol = Engine::loadFromFile("Game/assets/Assets/guns/pistolTest2/scene.gltf");

	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), sponza));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), pistol));

	std::cout << "Models created" << std::endl;
}
void FPSTest::loadOfflineEntities()
{
	// Pointers
	//Entity* entity;
	//CameraComponent* cameraComponent;
	//NetworkComponent* networkComponent;
	RenderComponent* renderComponent;
	PhysicsComponent* physicsComponent;

	EntityManager& entityManager = GetEntityManager();
	PhysicsWorld& physicsWorld = GetPhysicsWorld();

	std::vector<Engine::vk::Model>& models = GetModels();

	// Map
	std::vector<ComponentTypes> types = { RENDER, PHYSICS };
	mapEntity = entityManager.AddEntity(types);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(mapEntity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(0);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(mapEntity->GetEntityId(), PHYSICS));
	physicsComponent->initComplexShape(physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], mapEntity->GetModelMatrix(), mapEntity->GetEntityId());

	// Player 1
	types = { CAMERA, RENDER, PHYSICS };
	playerEntity = GetEntityManager().AddEntity(types);
	playerEntity->SetScale(0.5f);
	playerEntity->SetPosition(playerPos);




	//playerEntity->SetRotation(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::vec3 cameraPos = playerPos + glm::vec3(0.0f, 2.0f, 0.0f);

	CameraComponent* cameraComponent = reinterpret_cast<FPSCameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
	cameraComponent->SetCamera(&playerCam);
	renderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(1);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), PHYSICS));
	physicsComponent->init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[renderComponent->GetModelIndex()], playerEntity->GetModelMatrix(), playerEntity->GetEntityId());

	//// Pistol
	//types = { RENDER };
	//entity = entityManager.AddEntity(types);
	//entity->SetPosition(-1.0f, 1.0f, 0.0f);
	//entity->SetRotation(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//entity->SetScale(0.5f);
	//renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	//renderComponent->SetModelIndex(4);
}

void FPSTest::loadOnlineEntities()
{
	// Pointers
	Entity* entity;
	CameraComponent* cameraComponent;
	NetworkComponent* networkComponent;
	RenderComponent* renderComponent;
	PhysicsComponent* physicsComponent;

	EntityManager& entityManager = GetEntityManager();
	PhysicsWorld& physicsWorld = GetPhysicsWorld();

	std::vector<Engine::vk::Model>& models = GetModels();

	std::vector<ComponentTypes> types = { RENDER, PHYSICS };

	// Map
	entity = entityManager.AddEntity(types);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(0);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->initComplexShape(physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());

	// Helmet
	entity = entityManager.AddEntity(types);
	entity->SetPosition(0.0f, 2.0f, 0.0f);
	entity->SetRotation(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	entity->SetScale(0.2f);
	// configure physics component
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(1);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->init(physicsWorld, PhysicsComponent::PhysicsType::DYNAMIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());

	// Cube
	entity = entityManager.AddEntity(types);
	entity->SetPosition(0.3f, 1.0f, -1.0f);
	// configure physics component
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(2);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->init(physicsWorld, PhysicsComponent::PhysicsType::DYNAMIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());


	// Player 1
	types = { CAMERA, NETWORK, RENDER, PHYSICS };
	entity = entityManager.AddEntity(types);
	entity->SetPosition(-5.0f, 0.0f, -1.0f);
	entity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	entity->SetScale(30.0f);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(3);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());
	cameraComponent = reinterpret_cast<CameraComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), CAMERA));
	cameraComponent->SetCamera(new Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	networkComponent = reinterpret_cast<NetworkComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), NETWORK));
	networkComponent->SetClientId(0);

	types = { RENDER };

	// Player 2
	entity = entityManager.AddEntity(types);
	entity->SetPosition(5.0f, 0.0f, -1.0f);
	entity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	entity->SetScale(30.0f);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(3);

	// Pistol
	types = { RENDER };
	entity = entityManager.AddEntity(types);
	entity->SetPosition(-1.0f, 1.0f, 0.0f);
	entity->SetRotation(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	entity->SetScale(0.5f);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(4);

	std::vector<int> entitiesWithNetworkComponent = entityManager.GetEntitiesWithComponent(NETWORK);
	for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
	{
		networkComponent = reinterpret_cast<NetworkComponent*>(entityManager.GetComponentOfEntity(entitiesWithNetworkComponent[i], NETWORK));
		if (networkComponent->GetClientId() == clientId)
		{
			cameraComponent = reinterpret_cast<CameraComponent*>(entityManager.GetComponentOfEntity(entitiesWithNetworkComponent[i], CAMERA));
			GetRenderer().attachCamera(cameraComponent->GetCamera());
		}
	}
}

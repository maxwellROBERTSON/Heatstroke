#include "DemoGame.hpp"

#include <algorithm>
#include <chrono>
#include <future>
#include <GLFW/glfw3.h>
#include <imgui.h>
#include <random>
#include <thread>
#include <type_traits>

#include "../ECS/Components/AudioComponent.hpp"
#include "../Engine/Rendering/features/Skybox.hpp"
#include "../Engine/Rendering/PipelineCreation.hpp"
#include "../Engine/vulkan/objects/Buffer.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"
#include "Error.hpp"
#include "glm/gtx/string_cast.hpp"
#include "toString.hpp"

#include "gameGUI/GameGUI.hpp"
#include "gameModes/MultiPlayer.hpp"
#include "gameModes/SinglePlayer.hpp"

using namespace Engine;

void FPSTest::Init() {

	this->renderer = Renderer(&GetContext(), &GetEntityManager(), this);
	this->renderer.initialise();

	//this->gui = GUI(this);

	srand(time(0));

	//submit task to initialise Models to thread pool
	auto modelsFut = GetThreadPool().submit(&FPSTest::initialiseModels, this);

	DLOG("Waiting for model initialisation");
	//blocks execution of the rest of the program until the initialiseModels Thread has finished
	modelsFut.get();

	// Scene camera
	sceneCamera = Camera(60.0f, 0.01f, 1000.0f, glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	GetPhysicsWorld().init(&GetEntityManager());

	GetGUI().initGUI(getRenderer().getRenderPassHandle("gui"));
	makeGameGUIS(this);

	InputManager::InitDefaultControls();

	this->renderer.attachCamera(&sceneCamera);
	this->renderer.initialiseModelDescriptors();

	std::vector<const char*> skyboxFilenames = {
		"Game/assets/skybox/right.bmp",
		"Game/assets/skybox/left.bmp",
		"Game/assets/skybox/top.bmp",
		"Game/assets/skybox/bottom.bmp",
		"Game/assets/skybox/front.bmp",
		"Game/assets/skybox/back.bmp",
	};
	this->renderer.addSkybox(std::make_unique<Engine::Skybox>(&GetContext(), skyboxFilenames));

	this->crosshair = Crosshair(&GetContext());
	this->bulletDecals = Engine::Decals(&GetContext(), "Game/assets/decals/bullet_decal.png", 100);
}

void FPSTest::Render() {

	while (!glfwWindowShouldClose(this->GetContext().getGLFWWindow())) {
		Update();
	}

	this->renderer.finishRendering();
}

void FPSTest::Update() {

	Engine::InputManager::Update();

	GetNetwork().Update();

	// Need to process GUI stuff before checking swapchain, since
	// some GUI settings may require instant swapchain recreation
	GetGUI().makeGUI();

	if (this->renderer.checkSwapchain())
		return;

	if (this->renderer.acquireSwapchainImage())
		return;

	// Calculate time delta
	const auto now = std::chrono::steady_clock::now();
	const auto timeDelta = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - previous).count();
	previous = now;

	this->renderer.calculateFPS();

	if (this->renderer.getIsSceneLoaded())
	{
		if (gameMode)
		{
			gameMode->Update(timeDelta);
		}

		GetPhysicsWorld().updateObjects(GetModels());

		this->renderer.updateAnimations(timeDelta);
	}

	this->renderer.updateUniforms();
	this->renderer.render();
	this->renderer.submitRender();
}

void FPSTest::OnEvent(Engine::Event& e)
{
	Game::OnEvent(e);
	this->renderer.getCameraPointer()->OnEvent(this->GetContext().getGLFWWindow(), e);
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<ESCEvent>([this](Event& event) { toggleSettings(this); return true; });
	dispatcher.Dispatch<KeyPressedEvent>([&](KeyPressedEvent& event)
		{
			if (event.GetKeyCode() == HS_KEY_C) { GetGameMode().ToggleSceneCamera(&sceneCamera); }
			return true;
		});
}

void FPSTest::initialiseModels()
{
	// Here we would load all relevant glTF models and put them in the models vector

	tinygltf::Model map = Engine::loadFromFile("Game/assets/maps/warehouse/scene.gltf");
	tinygltf::Model character = Engine::loadFromFile("Game/assets/characters/csgo/scene.gltf");
	tinygltf::Model pistol = Engine::loadFromFile("Game/assets/guns/pistol1/scene.gltf");
	tinygltf::Model rifle = Engine::loadFromFile("Game/assets/guns/smg/scene.gltf");
	//tinygltf::Model rifle = Engine::loadFromFile("Game/assets/guns/rifle/scene.gltf");
	tinygltf::Model target = Engine::loadFromFile("Game/assets/target/scene.gltf");
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), map, DrawType::WORLD));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), character, DrawType::WORLD));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), pistol, DrawType::OVERLAY));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), rifle, DrawType::OVERLAY));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), target, DrawType::WORLD));

	DLOG("Models created.");
}

void FPSTest::loadOfflineEntities()
{
	// Pointers
	Entity* entity;
	CameraComponent* cameraComponent;
	RenderComponent* renderComponent;
	PhysicsComponent* physicsComponent;
	AudioComponent* audioComponent;

	EntityManager& entityManager = GetEntityManager();
	PhysicsWorld& physicsWorld = GetPhysicsWorld();

	std::vector<Engine::vk::Model>& models = GetModels();

	std::vector<ComponentTypes> types = { RENDER, PHYSICS };

	// map
	entity = entityManager.MakeNewEntity(types);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(0);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->InitComplexShape("Map", physicsWorld, PhysicsComponent::PhysicsType::STATICBOUNDED, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());

	// Character Model
	types = { CAMERA, RENDER, PHYSICS, AUDIO };
	entity = entityManager.MakeNewEntity(types);
	entity->SetSpawnState(GetGameMode().GetStartPos(0), 90.f, glm::vec3(0.0f, 0.0f, 1.0f), 24.f);
	cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), CAMERA));
	cameraComponent->SetCamera(sceneCamera);
	this->renderer.attachCamera(cameraComponent->GetCamera());
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(1);
	renderComponent->SetIsActive(false);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId(), true, true);
	audioComponent = reinterpret_cast<AudioComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), AUDIO));
	audioComponent->addClip("GunShot", "Game/assets/AudioClips/singlegunshot.wav");
	GetGameMode().SetPlayerEntity(entity);

	// secondary
	types = { RENDER };
	entity = entityManager.MakeNewEntity(types);
	entity->SetSpawnState(glm::vec3(0.0f, -2.0f, -1.0f), 180.0f, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(2);
	GetGameMode().SetPistolEntity(entity);

	// primary
	types = { RENDER };
	entity = entityManager.MakeNewEntity(types);
	entity->SetPosition(0.1f, -0.35f, 0.1f);
	entity->SetRotation(180.f, glm::vec3(0.0f, 1.0f, 0.0f));
	entity->SetScale(0.75f);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(3);
	renderComponent->SetIsActive(false);
	GetGameMode().SetRifleEntity(entity);

	// target
	types = { RENDER, PHYSICS };
	entity = entityManager.MakeNewEntity(types);
	entity->SetSpawnState(glm::vec3(-2.85f, 0.8f, 0.0f), 180, glm::normalize(glm::vec3(1, 0, -1)), 1.f);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(4);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->InitComplexShape("Target", physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());
	GetGameMode().SetTargetEntity(entity);
	GetEntityManager().ResetChanged();
}

void FPSTest::loadOnlineEntities(int maxClientsNum, int numTeams, bool isListenServer)
{
	// Pointers
	Entity* entity;
	CameraComponent* cameraComponent;
	NetworkComponent* networkComponent;
	RenderComponent* renderComponent;
	PhysicsComponent* physicsComponent;
	AudioComponent* audioComponent;
	ChildrenComponent* childrenComponent;

	EntityManager& entityManager = GetEntityManager();
	entityManager.SetNumTeams(numTeams);
	PhysicsWorld& physicsWorld = GetPhysicsWorld();

	std::vector<Engine::vk::Model>& models = GetModels();

	std::vector<ComponentTypes> types = { RENDER, PHYSICS };

	// Map
	entity = entityManager.MakeNewEntity(types);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(0);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->InitComplexShape("Map", physicsWorld, PhysicsComponent::PhysicsType::STATICBOUNDED, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());

	this->renderer.attachCamera(&sceneCamera);
	int start = 0;

	if (isListenServer)
	{
		// Load start server clients's player
		types = { CAMERA, RENDER, NETWORK, PHYSICS, AUDIO, CHILDREN };
		entity = entityManager.MakeNewEntity(types);
		entity->SetSpawnState(GetGameMode().GetStartPos(1), 90.0f, glm::vec3(0.0f, 0.0f, 1.0f), 24.f);
		cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), CAMERA));
		cameraComponent->SetCamera(sceneCamera);
		this->renderer.attachCamera(cameraComponent->GetCamera());
		renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
		renderComponent->SetModelIndex(1);
		renderComponent->SetIsActive(false);
		networkComponent = reinterpret_cast<NetworkComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), NETWORK));
		networkComponent->SetTeam(1);
		physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
		physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId(), true, true);
		audioComponent = reinterpret_cast<AudioComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), AUDIO));
		audioComponent->addClip("GunShot", "Game/assets/AudioClips/singlegunshot.wav");
		childrenComponent = reinterpret_cast<ChildrenComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), CHILDREN));

		entityManager.AssignNextClient(0, false);
		GetGameMode().SetPlayerEntity(entity);

		// pistol
		types = { RENDER };
		entity = entityManager.MakeNewEntity(types);
		entity->SetSpawnState(glm::vec3(0.0f, -2.0f, -1.0f), 180.0f, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
		renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
		renderComponent->SetModelIndex(2);

		childrenComponent->AddChild(entity->GetEntityId());

		GetGameMode().SetPistolEntity(entity);

		start++;
		maxClientsNum++;
	}

	// Load maxClientsNum of players
	for (int i = start; i < maxClientsNum; i++)
	{
		int team = i % numTeams + 1;
		// Character Model
		types = { CAMERA, RENDER, NETWORK, PHYSICS, AUDIO, CHILDREN };
		entity = entityManager.MakeNewEntity(types);
		entity->SetSpawnState(GetGameMode().GetStartPos(team), 90.0f, glm::vec3(0.0f, 0.0f, 1.0f), 24.f);
		cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), CAMERA));
		cameraComponent->SetCamera(sceneCamera);
		renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
		renderComponent->SetModelIndex(1);
		renderComponent->SetIsActive(false);
		networkComponent = reinterpret_cast<NetworkComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), NETWORK));
		networkComponent->SetTeam(team);
		physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
		physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId(), isListenServer, false);
		if (isListenServer)
		{
			physicsComponent->SetSimulation(PhysicsComponent::PhysicsSimulation::NOTUPDATED);
			entityManager.AddUpdatedPhysicsComp(physicsComponent, false);
		}
		audioComponent = reinterpret_cast<AudioComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), AUDIO));
		audioComponent->addClip("GunShot", "Game/assets/AudioClips/singlegunshot.wav");
		childrenComponent = reinterpret_cast<ChildrenComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), CHILDREN));

		// pistol
		types = { RENDER };
		entity = entityManager.MakeNewEntity(types);
		entity->SetSpawnState(glm::vec3(0.0f, -2.0f, -1.0f), 180.0f, glm::vec3(0.0f, 1.0f, 0.0f), 1.0f);
		renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
		renderComponent->SetModelIndex(2);

		childrenComponent->AddChild(entity->GetEntityId());
	}

	GetEntityManager().ResetChanged();
}

GameMode& FPSTest::GetGameMode()
{
	return *gameMode;
}

void FPSTest::SetGameMode(std::unique_ptr<GameMode> mode)
{
	gameMode = std::move(mode);
}

Renderer& FPSTest::getRenderer() {
	return this->renderer;
}

RenderMode FPSTest::getRenderMode() {
	return this->renderMode;
}

Crosshair& FPSTest::GetCrosshair() {
	return this->crosshair;
}

Decals& FPSTest::getBulletDecals() {
	return this->bulletDecals;
}
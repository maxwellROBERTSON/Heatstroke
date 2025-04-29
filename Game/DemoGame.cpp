#include "DemoGame.hpp"

#include <chrono>
#include <thread>
#include <future>
#include <type_traits>
#include <algorithm>

#include "../Engine/vulkan/objects/Buffer.hpp"
#include "../Engine/vulkan/PipelineCreation.hpp"
#include "../Engine/vulkan/Renderer.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"
#include "../Engine/vulkan/Skybox.hpp"
#include "../ECS/Components/AudioComponent.hpp"

#include "Error.hpp"
#include "toString.hpp"

#include <GLFW/glfw3.h>


using namespace Engine;

CameraComponent serverCameraComponent = CameraComponent(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

void FPSTest::Init()
{
	ac.addClip("Pain", "Game\\assets\\AudioClips\\Ugh.wav");
	ac.addClip("Dialogue", "Game\\assets\\AudioClips\\Moron.wav");
	ac.addClip("gunShot", "Game\\assets\\AudioClips\\singlegunshot.wav");*/
	

	this->threadPool = thread_pool_wait::get_instance();
 
	//submit task to initialise Models to thread pool
	auto modelsFut = threadPool->submit(&FPSTest::initialiseModels, this);

	std::cout << "Waiting for model initialisation" << std::endl;
	//blocks execution of the rest of the program until the initialiseModels Thread has finished
	modelsFut.get();

	GetPhysicsWorld().init(&GetEntityManager());
	GetRenderer().initialiseRenderer();
	GetGUI().initGUI();
	GetRenderer().attachCameraComponent(&serverCameraComponent);
	GetRenderer().initialiseModelDescriptors(GetModels());

	std::vector<const char*> skyboxFilenames = {
		"Game/assets/skybox/right.bmp",
		"Game/assets/skybox/left.bmp",
		"Game/assets/skybox/top.bmp",
		"Game/assets/skybox/bottom.bmp",
		"Game/assets/skybox/front.bmp",
		"Game/assets/skybox/back.bmp",
	};
	GetRenderer().addSkybox(std::make_unique<Engine::Skybox>(&GetContext(), skyboxFilenames));

	this->crosshair = Crosshair(&GetContext());
}

void FPSTest::Render()
{
	previous = std::chrono::steady_clock::now();

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
	if (!Engine::InputManager::mJoysticks.empty())
	{
		if (Engine::InputManager::getJoystick(0).isPressed(HS_GAMEPAD_BUTTON_A))
		{
			std::cout << "A BUTTON PRESSED" << std::endl;
		}
	}

<<<<<<<<< Temporary merge branch 1
	EntityManager& e = GetEntityManager();
=========
	auto keyboard = Engine::InputManager::getKeyboard();
	if (keyboard.isPressed(HS_KEY_W))
	{
		ac.playSound("Pain");
	}

	if (keyboard.isPressed(HS_KEY_S))
	{
		ac.playSound("Dialogue");
	}

	GetNetwork().Update();

	// Need to process GUI stuff before checking swapchain, since
	// some GUI settings may require instant swapchain recreation
	gui.makeGUI();

	if (renderer.checkSwapchain())
		return;

	if (renderer.acquireSwapchainImage())
		return;

	// Calculate time delta
	const auto now = std::chrono::steady_clock::now();
	const auto timeDelta = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - previous).count();
	previous = now;

	if (renderer.GetIsSceneLoaded())
	{
		renderer.calculateFPS();
		renderer.GetCameraComponentPointer()->UpdateCamera(this->GetContext().getGLFWWindow(), timeDelta);

		float fixedTimeDelta = std::min<float>(0.016f, timeDelta);

		physicsWorld.updatePhysics(timeDelta);
		physicsWorld.updateObjects(GetEntityManager(), GetModels());

		renderer.updateAnimations(timeDelta);
	}

	renderer.updateUniforms();
	renderer.render(GetModels());
	renderer.submitRender();
}

void FPSTest::OnEvent(Engine::Event& e)
{
	Game::OnEvent(e);

	GetRenderer().GetCameraPointer()->OnEvent(this->GetContext().getGLFWWindow(), e);
	//Engine::EventDispatcher dispatcher(e);

	//dispatcher.Dispatch<Engine::KeyPressedEvent>(
	//	[&](Engine::KeyPressedEvent& event)
	//	{
	//		std::cout << event.GetKeyCode() << std::endl;
	//		return true;
	//	}
	//);

	//dispatcher.Dispatch<Engine::MouseButtonPressedEvent>(
	//	[&](Engine::MouseButtonPressedEvent& event)
	//	{

	//		std::cout << event.GetMouseButton() << std::endl;
	//		return true;
	//	}
	//);
}

void FPSTest::initialiseModels()
{
	// Here we would load all relevant glTF models and put them in the models vector
	tinygltf::Model sponza = Engine::loadFromFile("Game/assets/Sponza/glTF/Sponza.gltf");
	tinygltf::Model map = Engine::loadFromFile("Game/assets/Sponza/glTF/Sponza.gltf");
	tinygltf::Model helmet = Engine::loadFromFile("Game/assets/DamagedHelmet.gltf");
	tinygltf::Model cube = Engine::loadFromFile("Game/assets/Cube.gltf");
	tinygltf::Model character = Engine::loadFromFile("Game/assets/Character/scene.gltf");
	tinygltf::Model pistol = Engine::loadFromFile("Game/assets/Assets/guns/pistol1/scene.gltf");
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), sponza));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), helmet));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), cube));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), character));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), pistol));

	std::cout << "Models created" << std::endl;
}

void FPSTest::loadOfflineEntities()
{
	// Pointers
	Entity* entity;
	CameraComponent* cameraComponent;
	NetworkComponent* networkComponent;
	RenderComponent* renderComponent;
	PhysicsComponent* physicsComponent;
	AudioComponent* audioComponent;

	EntityManager& entityManager = GetEntityManager();
	PhysicsWorld& physicsWorld = GetPhysicsWorld();

	std::vector<Engine::vk::Model>& models = GetModels();

	std::vector<ComponentTypes> types = { RENDER, PHYSICS };

	// Map
	entity = entityManager.MakeNewEntity(types);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(0);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->InitComplexShape(physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());
	entityManager.AddSimulatedPhysicsEntity(entity->GetEntityId());

	//// Helmet
	//entity = entityManager.MakeNewEntity(types);
	//entity->SetPosition(0.0f, 2.0f, 0.0f);
	//entity->SetRotation(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//entity->SetScale(0.2f);
	//// configure physics component
	//renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	//renderComponent->SetModelIndex(1);
	//physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	//physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::DYNAMIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId(), false, false);
	//entityManager.AddSimulatedPhysicsEntity(entity->GetEntityId());

	//// Cube
	//entity = entityManager.MakeNewEntity(types);
	//entity->SetPosition(0.3f, 1.0f, -1.0f);
	//// configure physics component
	//renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	//renderComponent->SetModelIndex(2);
	//physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	//physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::DYNAMIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId(), false, false);
	types = { AUDIO, CAMERA, NETWORK, RENDER, PHYSICS };

	// Player 1
	types = { CAMERA, NETWORK, RENDER, PHYSICS };
	entity = entityManager.MakeNewEntity(types);
	entity->SetPosition(-5.0f, 0.0f, -1.0f);
	entity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	entity->SetScale(30.0f);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(3); // changed
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId(), true, true);
	entityManager.AddSimulatedPhysicsEntity(entity->GetEntityId());
	cameraComponent = reinterpret_cast<CameraComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), CAMERA));
	cameraComponent->SetCamera(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	networkComponent = reinterpret_cast<NetworkComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), NETWORK));
	networkComponent->SetClientId(0);
	//add audio component to player
	audioComponent = reinterpret_cast<AudioComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), AUDIO));
	//add gunshot sound clip to the player for the gun 
	audioComponent->addClip("GunShot", "Game\\assets\\AudioClips\\singlegunshot.wav");



	//// Player 2
	//types = { RENDER };
	//entity = entityManager.MakeNewEntity(types);
	//entity->SetPosition(5.0f, 0.0f, -1.0f);
	//entity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	//entity->SetScale(30.0f);
	//renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	//renderComponent->SetModelIndex(3);

	//// Pistol
	//types = { RENDER };
	//entity = entityManager.MakeNewEntity(types);
	//entity->SetPosition(-1.0f, 1.0f, 0.0f);
	//entity->SetRotation(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//entity->SetScale(0.5f);
	//renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	//renderComponent->SetModelIndex(4);

	std::vector<int> entitiesWithNetworkComponent = entityManager.GetEntitiesWithComponent(NETWORK);
	for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
	{
		entity = entityManager.GetEntity(entitiesWithNetworkComponent[i]);
		networkComponent = reinterpret_cast<NetworkComponent*>(entityManager.GetComponentOfEntity(entitiesWithNetworkComponent[i], NETWORK));
		if (networkComponent->GetClientId() == offlineClientId)
		{
			cameraComponent = reinterpret_cast<CameraComponent*>(entityManager.GetComponentOfEntity(entitiesWithNetworkComponent[i], CAMERA));
			GetRenderer().attachCameraComponent(cameraComponent);
		}
	}

	GetEntityManager().ResetChanged();
}

void FPSTest::loadOnlineEntities(int maxClientsNum)
{
	// Pointers
	Entity* entity;
	CameraComponent* cameraComponent;
	// NetworkComponent* networkComponent;
	RenderComponent* renderComponent;
	PhysicsComponent* physicsComponent;

	EntityManager& entityManager = GetEntityManager();
	PhysicsWorld& physicsWorld = GetPhysicsWorld();

	std::vector<Engine::vk::Model>& models = GetModels();

	std::vector<ComponentTypes> types = { RENDER, PHYSICS };

	// Map
	entity = entityManager.MakeNewEntity(types);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(0);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->InitComplexShape(physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());
	entityManager.AddSimulatedPhysicsEntity(entity->GetEntityId());

	// Helmet
	//entity = entityManager.MakeNewEntity(types);
	//entity->SetPosition(0.0f, 2.0f, 0.0f);
	//entity->SetRotation(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//entity->SetScale(0.2f);
	//// configure physics component
	//renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	//renderComponent->SetModelIndex(1);
	//physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	//physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::DYNAMIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId(), false, false);
	//entityManager.AddSimulatedPhysicsEntity(entity->GetEntityId());

	// Cube
	//entity = entityManager.MakeNewEntity(types);
	//entity->SetPosition(0.3f, 1.0f, -1.0f);
	//// configure physics component
	//renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	//renderComponent->SetModelIndex(2);
	//physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	//physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::DYNAMIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId(), false, false);
	//entityManager.AddSimulatedPhysicsEntity(entity->GetEntityId());

	// Load maxClientsNum of players
	types = { AUDIO, CAMERA, NETWORK, RENDER, PHYSICS };
	for (int i = 0; i < maxClientsNum; i++)
	{
		entity = entityManager.MakeNewEntity(types);
		entity->SetPosition(-5.0f, 0.0f, -1.0f);
		entity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, -1.0f));
		entity->SetScale(30.0f);
		renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
		renderComponent->SetModelIndex(3);
		renderComponent->SetIsActive(false); // Players not renderable until client connection
		physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
		physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId(), false, false);
		cameraComponent = reinterpret_cast<CameraComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), CAMERA));
		cameraComponent->SetCamera(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	}

	//// Pistol
	//types = { RENDER };
	//entity = entityManager.MakeNewEntity(types);
	//entity->SetPosition(-1.0f, 1.0f, 0.0f);
	//entity->SetRotation(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//entity->SetScale(0.5f);
	//renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	//renderComponent->SetModelIndex(4);

	EntityManager& e = GetEntityManager();
	GetEntityManager().ResetChanged();
}

Crosshair& FPSTest::getCrosshair() {
	return this->crosshair;
}
#include "DemoGame.hpp"

#include <chrono>
#include <thread>
#include <future>
#include <algorithm>

#include "../Engine/vulkan/objects/Buffer.hpp"
#include "../Engine/vulkan/PipelineCreation.hpp"
#include "../Engine/vulkan/Renderer.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"

#include "Error.hpp"
#include "toString.hpp"

using namespace Engine;

Camera camera = Camera();

void FPSTest::Init()
{
	//create thread which then begins execution of initialiseModels
	std::thread initialiseModelsThread(&FPSTest::initialiseModels, this);

	std::cout << "Waiting for the execution of modelsThread to finish..." << std::endl;

	//blocks execution of the rest of the program until the initialiseModelsThread has finished
	initialiseModelsThread.join();
	GetPhysicsWorld().init();
	GetRenderer().initialiseRenderer();
	GetGUI().initGUI();
	GetRenderer().attachCamera(&camera);
	GetRenderer().initialiseModelDescriptors(GetModels());
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

	float fixedTimeDelta = std::min<float>(0.016f, timeDelta);
	
	physicsWorld.updateCharacter(fixedTimeDelta);
	// update PVD
	physicsWorld.gScene->simulate(fixedTimeDelta);
	physicsWorld.gScene->fetchResults(true);
	// update physics
	physicsWorld.updateObjects(GetEntityManager(), GetModels());

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
	cameraComponent->SetCamera(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	networkComponent = reinterpret_cast<NetworkComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), NETWORK));
	networkComponent->SetClientId(0);

	// Player 2
	types = { RENDER };
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
	cameraComponent->SetCamera(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	networkComponent = reinterpret_cast<NetworkComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), NETWORK));
	networkComponent->SetClientId(0);

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

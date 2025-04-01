#include "DemoGame.hpp"

#include <chrono>
#include <thread>
#include <future>

#include "../Engine/vulkan/objects/Buffer.hpp"
#include "../Engine/vulkan/PipelineCreation.hpp"
#include "../Engine/vulkan/Renderer.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"

#include "Error.hpp"
#include "toString.hpp"


void FPSTest::Init()
{
	registerComponents();
	//create thread which then begins execution of initialiseModels
	std::thread initialiseModelsThread(&FPSTest::initialiseModels, this);

	std::cout << "Waiting for the execution of modelsThread to finish..." << std::endl;

	//blocks execution of the rest of the program until the initialiseModelsThread has finished
	initialiseModelsThread.join();
	initialisePhysics();
}

void FPSTest::Update()
{
	RenderGUI();
}


void FPSTest::OnEvent(Engine::Event& e)
{
	Game::OnEvent(e);

	GetRenderer().GetCamera()->OnEvent(this->GetContext().getGLFWWindow(), e);
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

void FPSTest::registerComponents()
{
	// Register component types
	GetRegistry().RegisterComponentTypes<
		RenderComponent,
		PhysicsComponent,
		CameraComponent,
		NetworkComponent
	>();

	// Make vector of pointers to each component type's data
	std::vector<std::pair<void*, int>> componentTypePointers;

	// Push back the address of the component vectors
	componentTypePointers.push_back(std::make_pair(nullptr, 0));
	componentTypePointers.push_back(std::make_pair(nullptr, 0));
	componentTypePointers.push_back(std::make_pair(nullptr, 0));
	componentTypePointers.push_back(std::make_pair(nullptr, 0));

	// Add component pointers to the entity manager
	GetEntityManager().SetComponentTypesPointers(componentTypePointers);

	// Allocate entities with type vectors
	GetEntityManager().SetEntitiesWithType();
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

void FPSTest::initialisePhysics() 
{
	GetPhysicsWorld().init();
}

void FPSTest::RenderGUI()
{
	// Render loop for GUI before scene
	// For now just call RenderScene
	RenderScene();
}

void FPSTest::RenderScene()
{
	Engine::Camera camera = Engine::Camera();
	Engine::Renderer& renderer = GetRenderer();
	Engine::GUI& gui = GetGUI();
	PhysicsWorld& physicsWorld = GetPhysicsWorld();

	renderer.initialiseRenderer();
	gui.initGUI();
	renderer.attachCamera(&camera);
	renderer.initialiseModelDescriptors(GetModels());

	auto previous = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(this->GetContext().getGLFWWindow())) {
		Engine::InputManager::Update();
		if (!Engine::InputManager::mJoysticks.empty())
		{
			if (Engine::InputManager::getJoystick(0).isPressed(HS_GAMEPAD_BUTTON_A))
			{
				std::cout << "A BUTTON PRESSED" << std::endl;
			}
		}

		if (renderer.checkSwapchain())
			continue;

		if (renderer.acquireSwapchainImage())
			continue;

		// Calculate time delta
		const auto now = std::chrono::steady_clock::now();
		const auto timeDelta = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - previous).count();
		previous = now;

		renderer.GetCamera()->updateCamera(this->GetContext().getGLFWWindow(), timeDelta);

		float fixedDeltaTime = std::min(0.016f, timeDelta);

		physicsWorld.updateCharacter(fixedDeltaTime);
		// update PVD
		physicsWorld.gScene->simulate(fixedDeltaTime);
		physicsWorld.gScene->fetchResults(true);
		// update physics
		physicsWorld.updateObjects(GetEntityManager(), GetModels());

		renderer.updateAnimations(timeDelta);
		renderer.updateUniforms();
		gui.makeGUI();

		renderer.render(GetModels());
		renderer.submitRender();
	}

	renderer.finishRendering();
}

void FPSTest::loadOfflineEntities()
{
	// Pointers
	Entity* entity;
	RenderComponent* renderComponent;
	PhysicsComponent* physicsComponent;
	CameraComponent* cameraComponent;
	NetworkComponent* networkComponent;

	EntityManager& entityManager = GetEntityManager();
	PhysicsWorld& physicsWorld = GetPhysicsWorld();

	std::vector<Engine::vk::Model>& models = GetModels();

	// Map
	entity = entityManager.AddEntity<RenderComponent, PhysicsComponent>();
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(0);
	physicsComponent = entityManager.GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->initComplexShape(physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());

	// Helmet
	entity = entityManager.AddEntity<RenderComponent, PhysicsComponent>();
	entity->SetPosition(0.0f, 2.0f, 0.0f);
	entity->SetRotation(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	entity->SetScale(0.2f);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(1);
	// configure physics component
	physicsComponent = entityManager.GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->init(physicsWorld, PhysicsComponent::PhysicsType::DYNAMIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());

	// Cube
	entity = entityManager.AddEntity<RenderComponent, PhysicsComponent>();
	entity->SetPosition(0.3f, 1.0f, -1.0f);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(2);
	// configure physics component
	physicsComponent = entityManager.GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->init(physicsWorld, PhysicsComponent::PhysicsType::DYNAMIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());

	// Player 1
	entity = entityManager.AddEntity<RenderComponent, CameraComponent, NetworkComponent, PhysicsComponent>();
	entity->SetPosition(-5.0f, 0.0f, -1.0f);
	entity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	entity->SetScale(30.0f);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(3);
	physicsComponent = entityManager.GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());
	cameraComponent = entityManager.GetEntityComponent<CameraComponent>(entity->GetEntityId());
	cameraComponent->SetCamera(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	networkComponent = entityManager.GetEntityComponent<NetworkComponent>(entity->GetEntityId());
	networkComponent->SetClientId(0);

	// Player 2
	entity = entityManager.AddEntity<RenderComponent, CameraComponent, NetworkComponent>();
	entity->SetPosition(5.0f, 0.0f, -1.0f);
	entity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	entity->SetScale(30.0f);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(3);

	// Pistol
	entity = entityManager.AddEntity<RenderComponent>();
	entity->SetPosition(-1.0f, 1.0f, 0.0f);
	entity->SetRotation(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	entity->SetScale(0.5f);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(4);

	std::vector<int> entitiesWithNetworkComponent = entityManager.GetEntitiesWithComponent<NetworkComponent>();
	for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
	{
		networkComponent = entityManager.GetEntityComponent<NetworkComponent>(entitiesWithNetworkComponent[i]);
		if (networkComponent->GetClientId() == clientId)
		{
			GetRenderer().attachCamera(entityManager.GetEntityComponent<CameraComponent>(entitiesWithNetworkComponent[i])->GetCamera());
		}
	}
}
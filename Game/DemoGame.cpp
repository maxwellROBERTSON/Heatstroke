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
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), sponza));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), helmet));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), cube));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), character));

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

	GetRenderer().initialiseRenderer();
	GetGUI().initGUI();
	GetRenderer().attachCamera(&camera);
	GetRenderer().initialiseModelDescriptors(GetModels());	

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

		if (GetRenderer().checkSwapchain())
			continue;

		if (GetRenderer().acquireSwapchainImage())
			continue;

		// Calculate time delta
		const auto now = std::chrono::steady_clock::now();
		const auto timeDelta = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - previous).count();
		previous = now;

		GetRenderer().GetCamera()->updateCamera(this->GetContext().getGLFWWindow(), timeDelta);

		float fixedDeltaTime = std::min(0.016f, timeDelta);

		// update PVD
		GetPhysicsWorld().gScene->simulate(fixedDeltaTime);
		GetPhysicsWorld().gScene->fetchResults(true);
		// update physics
		GetPhysicsWorld().updateObjects(GetEntityManager(), GetModels());

		GetRenderer().updateUniforms();

		GetGUI().makeGUI();

		GetRenderer().render(GetModels());

		GetRenderer().submitRender();
	}

	GetRenderer().finishRendering();
}

void FPSTest::loadOfflineEntities()
{
	// Pointers
	Entity* entity;
	RenderComponent* renderComponent;
	PhysicsComponent* physicsComponent;
	CameraComponent* cameraComponent;
	NetworkComponent* networkComponent;

	// Map
	entity = GetEntityManager().AddEntity<RenderComponent>();
	glm::mat4 mapTransform(1.0f);
	mapTransform = glm::scale(mapTransform, glm::vec3(0.01f, 0.01f, 0.01f));
	entity->SetModelMatrix(mapTransform);
	renderComponent = GetEntityManager().GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(0);

	// Helmet
	entity = GetEntityManager().AddEntity<RenderComponent, PhysicsComponent>();
	glm::mat4 helmetTransform(1.0f);
	helmetTransform = glm::translate(helmetTransform, glm::vec3(0.0f, 2.0f, 0.0f));
	helmetTransform = glm::rotate(helmetTransform, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	helmetTransform = glm::rotate(helmetTransform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	helmetTransform = glm::scale(helmetTransform, glm::vec3(0.2f, 0.2f, 0.2f));
	entity->SetModelMatrix(helmetTransform);
	renderComponent = GetEntityManager().GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(1);
	// configure physics component
	physicsComponent = GetEntityManager().GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->init(GetPhysicsWorld(), PhysicsComponent::PhysicsType::DYNAMIC, helmetTransform, entity->GetEntityId());

	// Cube
	entity = GetEntityManager().AddEntity<RenderComponent, PhysicsComponent>();
	glm::mat4 cubeTransform(1.0f);
	cubeTransform = glm::translate(cubeTransform, glm::vec3(0.3f, 1.0f, -1.0f));
	cubeTransform = glm::scale(cubeTransform, glm::vec3(0.2f, 0.2f, 0.2f));
	entity->SetModelMatrix(cubeTransform);
	renderComponent = GetEntityManager().GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(2);
	// configure physics component
	physicsComponent = GetEntityManager().GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->init(GetPhysicsWorld(), PhysicsComponent::PhysicsType::DYNAMIC, cubeTransform, entity->GetEntityId());

	// Player 1
	entity = GetEntityManager().AddEntity<RenderComponent, CameraComponent ,NetworkComponent>();
	glm::mat4 player1Transform(1.0f);
	player1Transform = glm::translate(player1Transform, glm::vec3(-5.0f, 1.0f, -1.0f));
	player1Transform = glm::rotate(player1Transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	player1Transform = glm::scale(player1Transform, glm::vec3(1.0f, 1.0f, 1.0f));
	entity->SetModelMatrix(player1Transform);
	renderComponent = GetEntityManager().GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(3);
	//physicsComponent = GetEntityManager().GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	//physicsComponent->init(GetPhysicsWorld(), PhysicsComponent::PhysicsType::DYNAMIC, player1Transform, 3);
	cameraComponent = GetEntityManager().GetEntityComponent<CameraComponent>(entity->GetEntityId());
	cameraComponent->SetCamera(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	networkComponent = GetEntityManager().GetEntityComponent<NetworkComponent>(entity->GetEntityId());
	networkComponent->SetClientId(0);

	// Player 2
	entity = GetEntityManager().AddEntity<RenderComponent, CameraComponent, NetworkComponent>();
	glm::mat4 player2Transform(1.0f);
	player2Transform = glm::translate(player2Transform, glm::vec3(5.0f, 1.0f, -1.0f));
	player2Transform = glm::rotate(player2Transform, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	player2Transform = glm::scale(player2Transform, glm::vec3(1.0f, 1.0f, 1.0f));
	entity->SetModelMatrix(player2Transform);
	renderComponent = GetEntityManager().GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(3);
	//physicsComponent = GetEntityManager().GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	//physicsComponent->SetIsPerson(true

	std::vector<int> entitiesWithNetworkComponent = GetEntityManager().GetEntitiesWithComponent<NetworkComponent>();
	for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
	{
		networkComponent = GetEntityManager().GetEntityComponent<NetworkComponent>(entitiesWithNetworkComponent[i]);
		if (networkComponent->GetClientId() == clientId)
		{
			GetRenderer().attachCamera(GetEntityManager().GetEntityComponent<CameraComponent>(entitiesWithNetworkComponent[i])->GetCamera());
		}
	}
}

void FPSTest::loadOnlineEntities()
{
	// Pointers
	Entity* entity;
	RenderComponent* renderComponent;
	PhysicsComponent* physicsComponent;
	CameraComponent* cameraComponent;
	NetworkComponent* networkComponent;

	// Map
	entity = GetEntityManager().AddEntity<RenderComponent>();
	glm::mat4 mapTransform(1.0f);
	mapTransform = glm::scale(mapTransform, glm::vec3(0.01f, 0.01f, 0.01f));
	entity->SetModelMatrix(mapTransform);
	renderComponent = GetEntityManager().GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(0);

	// Helmet
	entity = GetEntityManager().AddEntity<RenderComponent, PhysicsComponent>();
	glm::mat4 helmetTransform(1.0f);
	helmetTransform = glm::translate(helmetTransform, glm::vec3(0.0f, 2.0f, 0.0f));
	helmetTransform = glm::rotate(helmetTransform, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	helmetTransform = glm::rotate(helmetTransform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	helmetTransform = glm::scale(helmetTransform, glm::vec3(0.2f, 0.2f, 0.2f));
	entity->SetModelMatrix(helmetTransform);
	renderComponent = GetEntityManager().GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(1);
	// configure physics component
	physicsComponent = GetEntityManager().GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->init(GetPhysicsWorld(), PhysicsComponent::PhysicsType::DYNAMIC, helmetTransform, entity->GetEntityId());

	// Cube
	entity = GetEntityManager().AddEntity<RenderComponent, PhysicsComponent>();
	glm::mat4 cubeTransform(1.0f);
	cubeTransform = glm::translate(cubeTransform, glm::vec3(0.3f, 1.0f, -1.0f));
	cubeTransform = glm::scale(cubeTransform, glm::vec3(0.2f, 0.2f, 0.2f));
	entity->SetModelMatrix(cubeTransform);
	renderComponent = GetEntityManager().GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(2);
	// configure physics component
	physicsComponent = GetEntityManager().GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->init(GetPhysicsWorld(), PhysicsComponent::PhysicsType::DYNAMIC, cubeTransform, entity->GetEntityId());

	// Player 1
	entity = GetEntityManager().AddEntity<RenderComponent, CameraComponent, NetworkComponent>();
	glm::mat4 player1Transform(1.0f);
	player1Transform = glm::translate(player1Transform, glm::vec3(-5.0f, 1.0f, -1.0f));
	player1Transform = glm::rotate(player1Transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	player1Transform = glm::scale(player1Transform, glm::vec3(1.0f, 1.0f, 1.0f));
	entity->SetModelMatrix(player1Transform);
	renderComponent = GetEntityManager().GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(3);
	//physicsComponent = GetEntityManager().GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	//physicsComponent->init(GetPhysicsWorld(), PhysicsComponent::PhysicsType::DYNAMIC, player1Transform, 3);
	cameraComponent = GetEntityManager().GetEntityComponent<CameraComponent>(entity->GetEntityId());
	cameraComponent->SetCamera(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	networkComponent = GetEntityManager().GetEntityComponent<NetworkComponent>(entity->GetEntityId());
	networkComponent->SetClientId(0);

	//// Player 2
	//entity = GetEntityManager().AddEntity<RenderComponent, CameraComponent, NetworkComponent>();
	//glm::mat4 player2Transform(1.0f);
	//player2Transform = glm::translate(player2Transform, glm::vec3(5.0f, 1.0f, -1.0f));
	//player2Transform = glm::rotate(player2Transform, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//player2Transform = glm::scale(player2Transform, glm::vec3(1.0f, 1.0f, 1.0f));
	//entity->SetModelMatrix(player2Transform);
	//renderComponent = GetEntityManager().GetEntityComponent<RenderComponent>(entity->GetEntityId());
	//renderComponent->SetModelIndex(3);
	////physicsComponent = GetEntityManager().GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	////physicsComponent->SetIsPerson(true

	std::vector<int> entitiesWithNetworkComponent = GetEntityManager().GetEntitiesWithComponent<NetworkComponent>();
	for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
	{
		networkComponent = GetEntityManager().GetEntityComponent<NetworkComponent>(entitiesWithNetworkComponent[i]);
		if (networkComponent->GetClientId() == clientId)
		{
			GetRenderer().attachCamera(GetEntityManager().GetEntityComponent<CameraComponent>(entitiesWithNetworkComponent[i])->GetCamera());
		}
	}
}

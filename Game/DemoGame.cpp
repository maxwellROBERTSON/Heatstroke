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

void FPSTest::Update()
{
	Engine::InputManager::Update();
	if (!Engine::InputManager::mJoysticks.empty())
	{
		if (Engine::InputManager::getJoystick(0).isPressed(HS_GAMEPAD_BUTTON_A))
		{
			std::cout << "A BUTTON PRESSED" << std::endl;
		}
	}

	GetNetwork().Update();

	if (GetRenderer().checkSwapchain())
		return;

	if (GetRenderer().acquireSwapchainImage())
		return;

	// Calculate time delta
	const auto now = std::chrono::steady_clock::now();
	const auto timeDelta = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - previous).count();
	previous = now;

	GetRenderer().GetCameraPointer()->updateCamera(this->GetContext().getGLFWWindow(), timeDelta);

	float fixedTimeDelta = std::min<float>(0.016f, timeDelta);
	
	GetPhysicsWorld().updateCharacter(fixedTimeDelta);
	// update PVD
	GetPhysicsWorld().gScene->simulate(fixedTimeDelta);
	GetPhysicsWorld().gScene->fetchResults(true);
	// update physics
	GetPhysicsWorld().updateObjects(GetEntityManager(), GetModels());

	GetRenderer().updateUniforms();

	GetGUI().makeGUI();

	GetRenderer().render(GetModels());

	GetRenderer().submitRender();
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
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), sponza));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), helmet));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), cube));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), character));

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

	std::vector<ComponentTypes> types = { RENDER, PHYSICS };

	// Map
	entity = GetEntityManager().AddEntity(types);
	glm::mat4 mapTransform(1.0f);
	mapTransform = glm::scale(mapTransform, glm::vec3(0.01f, 0.01f, 0.01f));
	entity->SetModelMatrix(mapTransform);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->initComplexShape(GetPhysicsWorld(), PhysicsComponent::PhysicsType::STATIC, GetModels()[0], mapTransform, entity->GetEntityId());
	renderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(0);

	// Helmet
	entity = GetEntityManager().AddEntity(types);
	glm::mat4 helmetTransform(1.0f);
	helmetTransform = glm::translate(helmetTransform, glm::vec3(0.0f, 2.0f, 0.0f));
	helmetTransform = glm::rotate(helmetTransform, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	helmetTransform = glm::rotate(helmetTransform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	helmetTransform = glm::scale(helmetTransform, glm::vec3(0.2f, 0.2f, 0.2f));
	entity->SetModelMatrix(helmetTransform);
	// configure physics component
	physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->init(GetPhysicsWorld(), PhysicsComponent::PhysicsType::DYNAMIC, helmetTransform, entity->GetEntityId());
	renderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(1);

	// Cube
	entity = GetEntityManager().AddEntity(types);
	glm::mat4 cubeTransform(1.0f);
	cubeTransform = glm::translate(cubeTransform, glm::vec3(0.3f, 1.0f, -1.0f));
	cubeTransform = glm::scale(cubeTransform, glm::vec3(0.4f, 0.4f, 0.4f));
	entity->SetModelMatrix(cubeTransform);
	// configure physics component
	physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->init(GetPhysicsWorld(), PhysicsComponent::PhysicsType::DYNAMIC, cubeTransform, entity->GetEntityId());
	renderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(2);

	// Player 1
	types = { CAMERA, NETWORK, RENDER, PHYSICS };
	entity = GetEntityManager().AddEntity(types);
	glm::mat4 player1Transform(1.0f);
	player1Transform = glm::translate(player1Transform, glm::vec3(-5.0f, 1.0f, -1.0f));
	player1Transform = glm::rotate(player1Transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	player1Transform = glm::scale(player1Transform, glm::vec3(1.0f, 1.0f, 1.0f));
	entity->SetModelMatrix(player1Transform);
	cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), CAMERA));
	cameraComponent->SetCamera(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	networkComponent = reinterpret_cast<NetworkComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), NETWORK));
	networkComponent->SetClientId(0);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->init(GetPhysicsWorld(), PhysicsComponent::PhysicsType::CONTROLLER, player1Transform, entity->GetEntityId());
	renderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(3);

	// Player 2
	entity = GetEntityManager().AddEntity(types);
	glm::mat4 player2Transform(1.0f);
	player2Transform = glm::translate(player2Transform, glm::vec3(5.0f, 1.0f, -1.0f));
	player2Transform = glm::rotate(player2Transform, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	player2Transform = glm::scale(player2Transform, glm::vec3(1.0f, 1.0f, 1.0f));
	entity->SetModelMatrix(player2Transform);
	renderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(3);
	//physicsComponent = GetEntityManager().GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	//physicsComponent->SetIsPerson(true

	std::vector<int> entitiesWithNetworkComponent = GetEntityManager().GetEntitiesWithComponent(NETWORK);
	for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
	{
		networkComponent = reinterpret_cast<NetworkComponent*>(GetEntityManager().GetComponentOfEntity(entitiesWithNetworkComponent[i], NETWORK));
		if (networkComponent->GetClientId() == clientId)
		{
			cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(entitiesWithNetworkComponent[i], CAMERA));
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

	std::vector<ComponentTypes> types = { RENDER, PHYSICS };

	// Map
	entity = GetEntityManager().AddEntity(types);
	glm::mat4 mapTransform(1.0f);
	mapTransform = glm::scale(mapTransform, glm::vec3(0.01f, 0.01f, 0.01f));
	entity->SetModelMatrix(mapTransform);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->initComplexShape(GetPhysicsWorld(), PhysicsComponent::PhysicsType::STATIC, GetModels()[0], mapTransform, entity->GetEntityId());
	renderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(0);

	// Helmet
	entity = GetEntityManager().AddEntity(types);
	glm::mat4 helmetTransform(1.0f);
	helmetTransform = glm::translate(helmetTransform, glm::vec3(0.0f, 2.0f, 0.0f));
	helmetTransform = glm::rotate(helmetTransform, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	helmetTransform = glm::rotate(helmetTransform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	helmetTransform = glm::scale(helmetTransform, glm::vec3(0.2f, 0.2f, 0.2f));
	entity->SetModelMatrix(helmetTransform);
	// configure physics component
	physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->init(GetPhysicsWorld(), PhysicsComponent::PhysicsType::DYNAMIC, helmetTransform, entity->GetEntityId());
	renderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(1);

	// Cube
	entity = GetEntityManager().AddEntity(types);
	glm::mat4 cubeTransform(1.0f);
	cubeTransform = glm::translate(cubeTransform, glm::vec3(0.3f, 1.0f, -1.0f));
	cubeTransform = glm::scale(cubeTransform, glm::vec3(0.4f, 0.4f, 0.4f));
	entity->SetModelMatrix(cubeTransform);
	// configure physics component
	physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->init(GetPhysicsWorld(), PhysicsComponent::PhysicsType::DYNAMIC, cubeTransform, entity->GetEntityId());
	renderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(2);

	// Player 1
	types = { CAMERA, NETWORK, RENDER, PHYSICS };
	entity = GetEntityManager().AddEntity(types);
	glm::mat4 player1Transform(1.0f);
	player1Transform = glm::translate(player1Transform, glm::vec3(-5.0f, 1.0f, -1.0f));
	player1Transform = glm::rotate(player1Transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	player1Transform = glm::scale(player1Transform, glm::vec3(1.0f, 1.0f, 1.0f));
	entity->SetModelMatrix(player1Transform);
	cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), CAMERA));
	cameraComponent->SetCamera(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	networkComponent = reinterpret_cast<NetworkComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), NETWORK));
	networkComponent->SetClientId(0);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->init(GetPhysicsWorld(), PhysicsComponent::PhysicsType::CONTROLLER, player1Transform, entity->GetEntityId());
	renderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(3);

	// Player 2
	entity = GetEntityManager().AddEntity(types);
	glm::mat4 player2Transform(1.0f);
	player2Transform = glm::translate(player2Transform, glm::vec3(5.0f, 1.0f, -1.0f));
	player2Transform = glm::rotate(player2Transform, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	player2Transform = glm::scale(player2Transform, glm::vec3(1.0f, 1.0f, 1.0f));
	entity->SetModelMatrix(player2Transform);
	renderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(3);
	//physicsComponent = GetEntityManager().GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	//physicsComponent->SetIsPerson(true

	std::vector<int> entitiesWithNetworkComponent = GetEntityManager().GetEntitiesWithComponent(NETWORK);
	for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
	{
		networkComponent = reinterpret_cast<NetworkComponent*>(GetEntityManager().GetComponentOfEntity(entitiesWithNetworkComponent[i], NETWORK));
		if (networkComponent->GetClientId() == clientId)
		{
			cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(entitiesWithNetworkComponent[i], CAMERA));
			GetRenderer().attachCamera(cameraComponent->GetCamera());
		}
	}
}

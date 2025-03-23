#include "DemoGame.h"

#include <chrono>


#include "../Engine/vulkan/objects/Buffer.hpp"
#include "../Engine/vulkan/PipelineCreation.hpp"
#include "../Engine/vulkan/Renderer.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"

#include "Error.hpp"
#include "toString.hpp"

void FPSTest::Init()
{
	std::cout << "FPS TEST INIT" << std::endl;
	registerComponents();
	initialiseModels();
	initialisePhysics(physics_world);
}

void FPSTest::Update()
{
	RenderGUI();
}

void FPSTest::OnEvent(Engine::Event& e)
{
	Engine::EventDispatcher dispatcher(e);

	dispatcher.Dispatch<Engine::KeyPressedEvent>(
		[&](Engine::KeyPressedEvent& event)
		{
			std::cout << event.GetKeyCode() << std::endl;
			return true;
		}
	);
}

void FPSTest::registerComponents()
{


	// Register component types
	registry.RegisterComponentTypes<
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
	entityManager.SetComponentTypesPointers(componentTypePointers);

	// Allocate entities with type vectors
	entityManager.SetEntitiesWithType();
}

void FPSTest::initialiseModels()
{
	// Here we would load all relevant glTF models and put them in the models vector
	tinygltf::Model sponza = Engine::loadFromFile("Game/assets/Sponza/glTF/Sponza.gltf");
	tinygltf::Model helmet = Engine::loadFromFile("Game/assets/DamagedHelmet.gltf");
	tinygltf::Model cube = Engine::loadFromFile("Game/assets/Cube.gltf");
	tinygltf::Model character = Engine::loadFromFile("Game/assets/Character/scene.gltf");
	models.emplace_back(Engine::makeVulkanModel(this->GetContext(), sponza));
	models.emplace_back(Engine::makeVulkanModel(this->GetContext(), helmet));
	models.emplace_back(Engine::makeVulkanModel(this->GetContext(), cube));
	models.emplace_back(Engine::makeVulkanModel(this->GetContext(), character));
}

void FPSTest::initialisePhysics(PhysicsWorld& pworld) 
{
	pworld.init();
}

void FPSTest::RenderGUI()
{
	// Render loop for GUI before scene
	// For now just call RenderScene
	RenderScene();
}

void FPSTest::RenderScene()
{
	if (online && isChange)
	{
		// Online mode
	}
	else if (offline && isChange)
	{
		// Offline mode
		isChange = false;
		loadOfflineEntities(registry, entityManager,physics_world);
		clientId = 0;
	}

	NetworkComponent* networkComponent;
	std::vector<int> entitiesWithNetworkComponent = entityManager.GetEntitiesWithComponent<NetworkComponent>();
	for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
	{
		networkComponent = entityManager.GetEntityComponent<NetworkComponent>(entitiesWithNetworkComponent[i]);
		if (networkComponent->GetClientId() == clientId)
		{
			camera = entityManager.GetEntityComponent<CameraComponent>(entitiesWithNetworkComponent[i])->GetCamera();
		}
	}

	this->renderer.initialiseRenderer();
	this->renderer.attachCamera(camera);
	this->renderer.initialiseModelDescriptors(models);	

	auto previous = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(this->GetContext().getGLFWWindow())) {
		glfwPollEvents();

		if (this->renderer.checkSwapchain())
			continue;

		if (this->renderer.acquireSwapchainImage())
			continue;

		// Calculate time delta
		const auto now = std::chrono::steady_clock::now();
		const auto timeDelta = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - previous).count();
		previous = now;

		camera->updateCamera(this->GetContext().getGLFWWindow(), timeDelta);

		float fixedDeltaTime = std::min(0.016f, timeDelta);

		// simulate controller first
		this->physics_world.updateCharacter(fixedDeltaTime);
		// then simulate all the dynamic objects
		this->physics_world.gScene->simulate(fixedDeltaTime);
		this->physics_world.gScene->fetchResults(true);

		
		// update entites with physics component
		this->physics_world.updateObjects(entityManager, models);


		this->renderer.updateUniforms();

		this->renderer.render(Engine::RenderMode::DEFERRED, models);

		this->renderer.submitRender();
	}

	this->renderer.finishRendering();
}

void loadOfflineEntities(ComponentTypeRegistry& registry, EntityManager& entityManager, PhysicsWorld& pworld)
{
	// Pointers
	Entity* entity;
	RenderComponent* renderComponent;
	PhysicsComponent* physicsComponent;
	CameraComponent* cameraComponent;
	NetworkComponent* networkComponent;

	// Map
	entity = entityManager.AddEntity<RenderComponent>();
	glm::mat4 mapTransform(1.0f);
	mapTransform = glm::scale(mapTransform, glm::vec3(0.01f, 0.01f, 0.01f));
	entity->SetModelMatrix(mapTransform);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(0);

	// Helmet
	entity = entityManager.AddEntity<RenderComponent, PhysicsComponent>();
	glm::mat4 helmetTransform(1.0f);
	helmetTransform = glm::translate(helmetTransform, glm::vec3(0.0f, 2.0f, 0.0f));
	helmetTransform = glm::rotate(helmetTransform, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	helmetTransform = glm::rotate(helmetTransform, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	helmetTransform = glm::scale(helmetTransform, glm::vec3(0.2f, 0.2f, 0.2f));
	entity->SetModelMatrix(helmetTransform);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(1);
	// configure physics component
	physicsComponent = entityManager.GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->init(pworld, PhysicsComponent::PhysicsType::DYNAMIC, helmetTransform, entity->GetEntityId());

	// Cube
	entity = entityManager.AddEntity<RenderComponent, PhysicsComponent>();
	glm::mat4 cubeTransform(1.0f);
	cubeTransform = glm::translate(cubeTransform, glm::vec3(0.3f, 1.0f, -1.0f));
	cubeTransform = glm::scale(cubeTransform, glm::vec3(0.4f, 0.4f, 0.4f));
	entity->SetModelMatrix(cubeTransform);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(2);
	// configure physics component
	physicsComponent = entityManager.GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->init(pworld, PhysicsComponent::PhysicsType::DYNAMIC, cubeTransform, entity->GetEntityId());


	// Player 1
	entity = entityManager.AddEntity<RenderComponent, CameraComponent ,NetworkComponent, PhysicsComponent>();
	glm::mat4 player1Transform(1.0f);
	player1Transform = glm::translate(player1Transform, glm::vec3(-5.0f, 1.0f, -1.0f));
	player1Transform = glm::rotate(player1Transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	player1Transform = glm::scale(player1Transform, glm::vec3(1.0f, 1.0f, 1.0f));
	entity->SetModelMatrix(player1Transform);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(3);
	physicsComponent = entityManager.GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->init(pworld, PhysicsComponent::PhysicsType::CONTROLLER, player1Transform, entity->GetEntityId());
	cameraComponent = entityManager.GetEntityComponent<CameraComponent>(entity->GetEntityId());
	cameraComponent->SetCamera(Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	networkComponent = entityManager.GetEntityComponent<NetworkComponent>(entity->GetEntityId());
	networkComponent->SetClientId(0);

	// Player 2
	entity = entityManager.AddEntity<RenderComponent, CameraComponent, NetworkComponent>();
	glm::mat4 player2Transform(1.0f);
	player2Transform = glm::translate(player2Transform, glm::vec3(5.0f, 1.0f, -1.0f));
	player2Transform = glm::rotate(player2Transform, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	player2Transform = glm::scale(player2Transform, glm::vec3(1.0f, 1.0f, 1.0f));
	entity->SetModelMatrix(player2Transform);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(3);
	//physicsComponent = entityManager.GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	//physicsComponent->SetIsPerson(true

}
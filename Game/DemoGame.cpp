#include "DemoGame.hpp"

#include <algorithm>
#include <chrono>
#include <future>
#include <random>
#include <thread>
#include <type_traits>
#include <GLFW/glfw3.h>
#include <imgui.h>

#include "../ECS/Components/AudioComponent.hpp"
#include "../Engine/vulkan/objects/Buffer.hpp"
#include "../Engine/vulkan/PipelineCreation.hpp"
#include "../Engine/vulkan/Renderer.hpp"
#include "../Engine/vulkan/Skybox.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"
#include "Error.hpp"
#include "glm/gtx/string_cast.hpp"
#include "toString.hpp"

//#include "glm/gtc/random.hpp" // breaks?
std::uniform_int_distribution<> randomDistrib(1, 7);
std::random_device rd;  // a seed source for the random number engine
std::mt19937 gen(rd()); // mersenne_twister_engine seeded with rd()

using namespace Engine;

float fireDelay = 1.0f;
bool canFire = true;
float counter = 1.0f;

void FPSTest::Init()
{
	srand(time(0));
	this->threadPool = thread_pool_wait::get_instance();

	//submit task to initialise Models to thread pool
	auto modelsFut = threadPool->submit(&FPSTest::initialiseModels, this);

	std::cout << "Waiting for model initialisation" << std::endl;
	//blocks execution of the rest of the program until the initialiseModels Thread has finished
	modelsFut.get();

	// Camera
	sceneCam = Camera(60.0f, 0.01f, 1000.0f, glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 1.0f));

	GetPhysicsWorld().init(&GetEntityManager());

	GetRenderer().initialiseRenderer();

	GetGUI().initGUI();
	GetRenderer().attachCameraComponent(new CameraComponent(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
	GetRenderer().initialiseModelDescriptors();

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

void FPSTest::Render() {

	while (!glfwWindowShouldClose(this->GetContext().getGLFWWindow())) {
		Update();
	}

	GetRenderer().finishRendering();
}

void FPSTest::Update() {

	Engine::Renderer& renderer = GetRenderer();
	Engine::PhysicsWorld& physicsWorld = GetPhysicsWorld();
	Engine::GUI& gui = GetGUI();

	Engine::InputManager::Update();

	EntityManager& e = GetEntityManager();

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

	renderer.calculateFPS();

	if (renderer.GetIsSceneLoaded())
	{
		renderer.GetCameraComponentPointer()->UpdateCamera(this->GetContext().getGLFWWindow(), timeDelta);

		float fixedTimeDelta = std::min<float>(0.016f, timeDelta);

		fireDelay -= timeDelta;
		counter -= timeDelta;
		if (fireDelay <= 0.0f)
			canFire = true;

		if (counter <= 0.0f && countdown > 0)
		{
			countdown--;
			counter = 1.0f;
		}

		CameraComponent* cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));

		if (playerEntity != nullptr && cameraComponent->GetCamera()->camMode == CameraMode::PLAYER)
		{
			physicsWorld.updatePhysics(playerEntity, timeDelta);
			cameraComponent->playerEntity = playerEntity;
			cameraComponent->SetCameraOffset(cameraOffset);
			playerEntity->frontDirection = cameraComponent->GetFrontDirection();
			playerEntity->frontDirection.y = 0.0f;
			playerEntity->frontDirection = glm::normalize(playerEntity->frontDirection);
			cameraComponent->UpdateCameraPosition(playerEntity->GetPosition());
		}
		else
		{
			physicsWorld.updatePhysics(nullptr, timeDelta);
		}

		if (Engine::InputManager::getMouse().isPressed(HS_MOUSE_BUTTON_LEFT) && canFire)
		{
			RenderComponent* pistolRenderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(pistolEntity->GetEntityId(), RENDER));
			AudioComponent* playerAudioComponent = reinterpret_cast<AudioComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), AUDIO));
			int pistolModelIndex = pistolRenderComponent->GetModelIndex();
			std::vector<vk::Model>& models = GetModels();
			models[pistolModelIndex].animationQueue.push(models[pistolModelIndex].animations[3]);
			models[pistolModelIndex].blending = true;
			canFire = false;
			fireDelay = 1.0f;
			PxRaycastHit entityHit = physicsWorld.handleShooting(playerEntity);
			std::vector<int> entitiesWithPhysicsComponent = GetEntityManager().GetEntitiesWithComponent(PHYSICS);
			for (int i = 0; i < entitiesWithPhysicsComponent.size(); i++)
			{
				Entity* entity = GetEntityManager().GetEntity(entitiesWithPhysicsComponent[i]);
				PhysicsComponent* physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
				if (physicsComponent->GetStaticBody() != nullptr && physicsComponent->GetStaticBody() == entityHit.actor)
				{
					score++;

					RenderComponent* hitRenderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(entity->GetEntityId(), RENDER));
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

		physicsWorld.updateObjects(GetEntityManager(), GetModels());

		renderer.updateAnimations(timeDelta);
	}

	renderer.updateUniforms();
	renderer.render();
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
				CameraComponent* cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
				cameraComponent->swapCameraMode();
			}
			return true;
		});
}

void FPSTest::DrawGUI()
{
	CameraComponent* cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
	ImGuiWindowFlags window_flags = 0;
	window_flags |= ImGuiWindowFlags_NoBackground;
	window_flags |= ImGuiWindowFlags_NoTitleBar;
	//ImGui::ShowStyleEditor();
	//ImGui::GetIO().Fonts->AddFontFromFileTTF("C:/dev/Heatstroke/Engine/third_party/imgui/misc/fonts/Roboto-Medium.ttf", 36.0f);
	bool test = true;
	ImGui::Begin("Game:", &test, window_flags);
	//ImGui::PushFont(GetGUI().gameFont);
	ImGui::Text("SCORE: %u", score);
	ImGui::Text("Time: %u", countdown);

	//ImGui::PopFont();

	ImGui::End();
}

void FPSTest::DrawDebugGUI()
{
	ImGui::Begin("Game Debug");
	ImGui::Checkbox("Game GUI: ", &showGUI);
	if (showGUI)
	{
		ImGui::PushFont(GetGUI().gameFont);
		DrawGUI();
		ImGui::PopFont();
	}
	if (ImGui::Button("Reset Game"))
	{
		countdown = 31;
		score = 0;
	}
	ImGui::End();
}

void FPSTest::initialiseModels()
{
	/* tinygltf::Model sponza = Engine::loadFromFile("Game/assets/Sponza/glTF/Sponza.gltf");
	tinygltf::Model map = Engine::loadFromFile("Game/assets/Assets/maps/breeze/scene.gltf");
	tinygltf::Model map2 = Engine::loadFromFile("Game/assets/Assets/maps/chamberOfSecrets/scene.gltf");
	tinygltf::Model map3 = Engine::loadFromFile("Game/assets/Assets/maps/house/scene.gltf");
	tinygltf::Model map4 = Engine::loadFromFile("Game/assets/Assets/maps/map/scene.gltf");
	tinygltf::Model map5 = Engine::loadFromFile("Game/assets/Assets/maps/pipeline/scene.gltf");
	tinygltf::Model map6 = Engine::loadFromFile("Game/assets/Assets/maps/russian_house/scene.gltf");
	tinygltf::Model map7 = Engine::loadFromFile("Game/assets/Assets/maps/standoff/scene.gltf");
	tinygltf::Model map8 = Engine::loadFromFile("Game/assets/Assets/maps/uncharted/scene.gltf");
	tinygltf::Model map9 = Engine::loadFromFile("Game/assets/Assets/maps/warehouse/scene.gltf"); */

	// Here we would load all relevant glTF models and put them in the models vector

	//tinygltf::Model map = Engine::loadFromFile("Game/assets/Assets/maps/warehouse/scene.gltf");
	tinygltf::Model map = Engine::loadFromFile("Game/assets/Sponza/glTF/Sponza.gltf");
	tinygltf::Model character = Engine::loadFromFile("Game/assets/Character/scene.gltf");
	tinygltf::Model pistol = Engine::loadFromFile("Game/assets/Assets/guns/pistol1/scene.gltf");
	tinygltf::Model rifle = Engine::loadFromFile("Game/assets/Assets/guns/rifle/scene.gltf");
	tinygltf::Model target = Engine::loadFromFile("Game/assets/target/scene.gltf");
	tinygltf::Model helmet = Engine::loadFromFile("Game/assets/DamagedHelmet.gltf");
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), map, DrawType::WORLD));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), character, DrawType::WORLD));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), pistol, DrawType::OVERLAY));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), rifle, DrawType::OVERLAY));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), target, DrawType::WORLD));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), helmet, DrawType::WORLD));

	std::cout << "Models created" << std::endl;
}

void FPSTest::loadOfflineEntities()
{
	// Pointers
	CameraComponent* cameraComponent;
	RenderComponent* renderComponent;
	PhysicsComponent* physicsComponent;
	NetworkComponent* networkComponent;
	AudioComponent* audioComponent;

	EntityManager& entityManager = GetEntityManager();
	PhysicsWorld& physicsWorld = GetPhysicsWorld();

	std::vector<Engine::vk::Model>& models = GetModels();

	// map
	std::vector<ComponentTypes> types = { RENDER, PHYSICS };
	mapEntity = entityManager.MakeNewEntity(types);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(mapEntity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(0);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(mapEntity->GetEntityId(), PHYSICS));
	physicsComponent->InitComplexShape("Map", physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], mapEntity->GetModelMatrix(), mapEntity->GetEntityId());
	entityManager.AddSimulatedPhysicsEntity(mapEntity->GetEntityId());
	
	// Character Model
	types = { AUDIO, CAMERA, NETWORK, PHYSICS };
	playerEntity = entityManager.MakeNewEntity(types);
	playerEntity->SetPosition(playerPos);

	cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
	cameraComponent->SetCamera(sceneCam);
	cameraComponent->playerEntity = playerEntity;
	physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), PHYSICS));
	physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[1], playerEntity->GetModelMatrix(), playerEntity->GetEntityId(), true, true);
	entityManager.AddSimulatedPhysicsEntity(playerEntity->GetEntityId());
	//add audio component to player
	audioComponent = reinterpret_cast<AudioComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), AUDIO));

	networkComponent = reinterpret_cast<NetworkComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), NETWORK));
	networkComponent->SetClientId(0);
	//add gunshot sound clip to the player for the gun 
	audioComponent->addClip("GunShot", "Game\\assets\\AudioClips\\singlegunshot.wav");
	GetRenderer().attachCameraComponent(cameraComponent);

	// pistol
	types = { RENDER };
	pistolEntity = entityManager.MakeNewEntity(types);
	pistolEntity->SetPosition(0.0f, -2.0f, -1.0f);
	pistolEntity->SetRotation(180.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(pistolEntity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(2);

	// targets
	types = { RENDER, PHYSICS };
	targetEntity = entityManager.MakeNewEntity(types);
	targetEntity->SetPosition(glm::vec3(3.0f, 1.0f, 0.0f));
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(targetEntity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(4);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(targetEntity->GetEntityId(), PHYSICS));
	physicsComponent->InitComplexShape("Target", physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], targetEntity->GetModelMatrix(), targetEntity->GetEntityId());
	entityManager.AddSimulatedPhysicsEntity(targetEntity->GetEntityId());
	networkComponent->SetClientId(0);

	int numberOfTargets = 5;
	//targetEntity1 = entityManager.MakeNewEntity(types);
	//targetEntity1->SetPosition(target1Pos);
	//renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(targetEntity1->GetEntityId(), RENDER));
	//renderComponent->SetModelIndex(6);
	//physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(targetEntity1->GetEntityId(), PHYSICS));
	//physicsComponent->InitComplexShape("Target", physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], targetEntity1->GetModelMatrix(), targetEntity1->GetEntityId());
	//entityManager.AddSimulatedPhysicsEntity(targetEntity1->GetEntityId());
	//renderComponent->SetModelIndex(3);

	//targetEntity2 = entityManager.MakeNewEntity(types);
	//targetEntity2->SetPosition(target2Pos);
	//renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(targetEntity2->GetEntityId(), RENDER));
	//renderComponent->SetModelIndex(6);
	//physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(targetEntity2->GetEntityId(), PHYSICS));
	//physicsComponent->InitComplexShape("Target", physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], targetEntity2->GetModelMatrix(), targetEntity2->GetEntityId());
	//entityManager.AddSimulatedPhysicsEntity(targetEntity2->GetEntityId());

	//targetEntity3 = entityManager.MakeNewEntity(types);
	//targetEntity3->SetPosition(target3Pos);
	//renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(targetEntity3->GetEntityId(), RENDER));
	//renderComponent->SetModelIndex(6);
	//physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(targetEntity3->GetEntityId(), PHYSICS));
	//physicsComponent->InitComplexShape("Target", physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], targetEntity3->GetModelMatrix(), targetEntity3->GetEntityId());
	//entityManager.AddSimulatedPhysicsEntity(targetEntity3->GetEntityId());
	std::vector<int> entitiesWithNetworkComponent = entityManager.GetEntitiesWithComponent(NETWORK);
	for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
	{
		playerEntity = entityManager.GetEntity(entitiesWithNetworkComponent[i]);
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

	// ---- input actions ----- 

	// Player 1
	// types = { CAMERA, NETWORK, RENDER, PHYSICS };
	// entity = entityManager.AddEntity(types);
	// entity->SetPosition(-5.0f, 0.0f, -1.0f);
	// entity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	// entity->SetScale(30.0f);
	// renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	// renderComponent->SetModelIndex(3);
	// physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	// physicsComponent->init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId());
	// cameraComponent = reinterpret_cast<CameraComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), CAMERA));
	// cameraComponent->SetCamera(new Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	// networkComponent = reinterpret_cast<NetworkComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), NETWORK));
	// networkComponent->SetClientId(0);

	// types = { RENDER };

	// // Player 2
	// entity = entityManager.AddEntity(types);
	// entity->SetPosition(5.0f, 0.0f, -1.0f);
	// entity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	// entity->SetScale(30.0f);
	// renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	// renderComponent->SetModelIndex(3);

	// // Pistol
	// types = { RENDER };
	// entity = entityManager.AddEntity(types);
	// entity->SetPosition(-1.0f, 1.0f, 0.0f);
	// entity->SetRotation(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	// entity->SetScale(0.5f);
	// renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	// renderComponent->SetModelIndex(4);

	// std::vector<int> entitiesWithNetworkComponent = entityManager.GetEntitiesWithComponent(NETWORK);
	// for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)

	// ---- input actions ----- 

	// Load maxClientsNum of players
	types = { AUDIO, CAMERA, NETWORK, RENDER, PHYSICS };
	for (int i = 0; i < maxClientsNum; i++)
	{
		entity = entityManager.MakeNewEntity(types);
		entity->SetPosition(-5.0f, 0.0f, -1.0f);
		entity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, -1.0f));
		entity->SetScale(30.0f);
		renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
		renderComponent->SetModelIndex(1);
		renderComponent->SetIsActive(false); // Players not renderable until client connection
		physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
		physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId(), false, false);
		cameraComponent = reinterpret_cast<CameraComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), CAMERA));
		cameraComponent->SetCamera(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 20.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	}

	EntityManager& e = GetEntityManager();
	GetEntityManager().ResetChanged();
}

Crosshair& FPSTest::getCrosshair() {
	return this->crosshair;
}
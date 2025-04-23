#include "DemoGame.hpp"

#include <algorithm>
#include <chrono>
#include <future>
#include <thread>
#include <type_traits>

#include "../Engine/vulkan/objects/Buffer.hpp"
#include "../Engine/vulkan/PipelineCreation.hpp"
#include "../Engine/vulkan/Renderer.hpp"
#include "../Engine/vulkan/Skybox.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"

#include "Error.hpp"
#include "glm/gtx/string_cast.hpp"
#include "toString.hpp"

#include <GLFW/glfw3.h>

#include <imgui.h>

using namespace Engine;

float fireDelay = 1.0f;
//bool canFire = true;
bool canFire = false;

//serverCameraComponent = CameraComponent(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));

void FPSTest::Init()
{
	this->threadPool = thread_pool_wait::get_instance();

	//submit task to initialise Models to thread pool
	auto modelsFut = threadPool->submit(&FPSTest::initialiseModels, this);

	std::cout << "Waiting for model initialisation" << std::endl;
	//blocks execution of the rest of the program until the initialiseModels Thread has finished
	modelsFut.get();

	// // Cameras
	sceneCam = Camera(60.0f, 0.01f, 1000.0f, glm::vec3(0.0f, 0.0f, -5.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//playerCam = FPSCamera("Player Cam", 100.0f, 0.01f, 256.0f, playerPos, glm::vec3(0.0f, 0.0f, 1.0f));
	//fpsCameraComponent.SetCamera(playerCam);
	//serverCameraComponent = CameraComponent(sceneCam);
	//FPSCameraComponent = 
	// playerEntity = 0;
	// mapEntity = 0;

	GetPhysicsWorld().init();
	GetRenderer().initialiseRenderer();
	GetGUI().initGUI();
	GetRenderer().attachCameraComponent(new CameraComponent(Engine::Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f))));
	//GetRenderer().attachCameraComponent(&serverCameraComponent);
	//GetRenderer().attachCameraComponent(&fpsCameraComponent);
	GetRenderer().initialiseModelDescriptors(GetModels());


	// GetRenderer().attachCamera(&playerCam);
	//GetRenderer().attachCamera(&sceneCam);

	std::vector<const char*> skyboxFilenames = {
		"Game/assets/skybox/right.bmp",
		"Game/assets/skybox/left.bmp",
		"Game/assets/skybox/top.bmp",
		"Game/assets/skybox/bottom.bmp",
		"Game/assets/skybox/front.bmp",
		"Game/assets/skybox/back.bmp",
	};
	GetRenderer().addSkybox(std::make_unique<Engine::Skybox>(&GetContext(), skyboxFilenames));
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

	if (renderer.GetIsSceneLoaded())
	{
		renderer.calculateFPS();
		renderer.GetCameraComponentPointer()->UpdateCamera(this->GetContext().getGLFWWindow(), timeDelta);

		float fixedTimeDelta = std::min<float>(0.016f, timeDelta);

		physicsWorld.updatePhysics(timeDelta);

		CameraComponent* cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));

		if (playerEntity != nullptr && cameraComponent->GetCamera()->camMode == CameraMode::PLAYER)
		{
			cameraComponent->SetCameraOffset(cameraOffset);
			playerEntity->frontDirection = cameraComponent->GetFrontDirection();
			playerEntity->frontDirection.y = 0.0f;

			glm::mat4 baseRotation = glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
			glm::mat4 poseRotation = glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 camRotation = glm::rotate(glm::radians(-cameraComponent->GetCamera()->yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
			glm::mat4 finalRotation = camRotation * poseRotation * baseRotation;
			physicsWorld.updateCharacter(playerEntity, timeDelta);
			playerEntity->SetRotation(finalRotation);
			cameraComponent->UpdateCameraPosition(playerEntity->GetPosition());


			//playerPos = playerEntity->GetPosition();
			//	////playerPos -= glm::vec3(0.0f, 0.0f, 0.5f);
			//	//playerPos -= glm::vec3(0.0f, 1.0f, 0.0f);
			//	//playerEntity->SetPosition(playerPos);
			//glm::mat4 playerRotation = playerEntity->GetRotation();
			//glm::mat4 standUp = glm::rotate(glm::radians(90.0f), glm::vec3(1, 0, 0));

			//glm::mat4 playerRotation = glm::mat4(1.0f);
			//playerRotation = glm::rotate(playerRotation, glm::radians(-cameraComponent->GetCamera()->yaw), glm::vec3(0.0f, 1.0f, 0.0f));
			//playerRotation = glm::rotate(playerRotation, glm::radians(-cameraComponent->GetCamera()->pitch), glm::vec3(1.0f, 0.0f, 0.0f));
			//	//playerPos = playerEntity->GetPosition();
			//	//playerPos += glm::vec3(0.0f, 0.0f, 0.5f);
			//	//playerPos += glm::vec3(0.0f, 1.0f, 0.0f);
			//	//playerEntity->SetPosition(playerPos);
			//	//glm::vec3 cameraPos = playerEntity->GetPosition();
		}
		//	if (camMode == CameraMode::PLAYER && playerEntity != nullptr)
//	{
//		physicsWorld.updateCharacter(playerEntity, fixedTimeDelta);

//		PhysicsComponent* physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), PHYSICS));
//		FPSCameraComponent* cameraComponent = reinterpret_cast<FPSCameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
//		cameraComponent->SetCameraOffset(cameraOffset);
//		//std::cout << glm::to_string(cameraPos) << std::endl;

//		playerEntity->frontDirection = cameraComponent->GetFrontDirection();
//		playerEntity->frontDirection.y = 0.0f;
//		//playerPos = playerEntity->GetPosition();
//		////playerPos -= glm::vec3(0.0f, 0.0f, 0.5f);
//		//playerPos -= glm::vec3(0.0f, 1.0f, 0.0f);
//		//playerEntity->SetPosition(playerPos);
//		glm::mat4 playerRotation = glm::mat4(1.0f);
//		playerRotation = glm::rotate(playerRotation, glm::radians(-cameraComponent->GetCamera()->yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
//		playerRotation = glm::rotate(playerRotation, glm::radians(-cameraComponent->GetCamera()->pitch), glm::vec3(1.0f, 0.0f, 0.0f));
//		playerEntity->SetRotation(playerRotation);
//		//playerPos = playerEntity->GetPosition();
//		//playerPos += glm::vec3(0.0f, 0.0f, 0.5f);
//		//playerPos += glm::vec3(0.0f, 1.0f, 0.0f);
//		//playerEntity->SetPosition(playerPos);
//		glm::vec3 cameraPos = playerEntity->GetPosition();
//		cameraComponent->UpdateCameraPosition(cameraPos);

		physicsWorld.updateObjects(GetEntityManager(), GetModels());

		renderer.updateAnimations(timeDelta);
		//switch (camMode)
		//{
		//	case(CameraMode::SCENE):
		//		GetRenderer().attachCamera(&sceneCam);
		//		break;
		//	case(CameraMode::PLAYER):
		//		GetRenderer().attachCamera(&playerCam);
		//		break;
		//	default:
		//		GetRenderer().attachCamera(&sceneCam);
		//		break;
		//}


	//fireDelay -= fixedTimeDelta;
	//if (fireDelay <= 0.0f)
	//	canFire = true;

	// update PVD
	// physicsWorld.gScene->simulate(fixedTimeDelta);
	// physicsWorld.gScene->fetchResults(true);
	// // update physics
	// physicsWorld.updateObjects(GetEntityManager(), GetModels());

	//	if (camMode == CameraMode::PLAYER && playerEntity != nullptr)
	//	{
	//		physicsWorld.updateCharacter(playerEntity, fixedTimeDelta);

	//		PhysicsComponent* physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), PHYSICS));
	//		FPSCameraComponent* cameraComponent = reinterpret_cast<FPSCameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
	//		cameraComponent->SetCameraOffset(cameraOffset);
	//		//std::cout << glm::to_string(cameraPos) << std::endl;

	//		playerEntity->frontDirection = cameraComponent->GetFrontDirection();
	//		playerEntity->frontDirection.y = 0.0f;
	//		//playerPos = playerEntity->GetPosition();
	//		////playerPos -= glm::vec3(0.0f, 0.0f, 0.5f);
	//		//playerPos -= glm::vec3(0.0f, 1.0f, 0.0f);
	//		//playerEntity->SetPosition(playerPos);
	//		glm::mat4 playerRotation = glm::mat4(1.0f);
	//		playerRotation = glm::rotate(playerRotation, glm::radians(-cameraComponent->GetCamera()->yaw + 90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//		playerRotation = glm::rotate(playerRotation, glm::radians(-cameraComponent->GetCamera()->pitch), glm::vec3(1.0f, 0.0f, 0.0f));
	//		playerEntity->SetRotation(playerRotation);
	//		//playerPos = playerEntity->GetPosition();
	//		//playerPos += glm::vec3(0.0f, 0.0f, 0.5f);
	//		//playerPos += glm::vec3(0.0f, 1.0f, 0.0f);
	//		//playerEntity->SetPosition(playerPos);
	//		glm::vec3 cameraPos = playerEntity->GetPosition();
	//		cameraComponent->UpdateCameraPosition(cameraPos);


		if (Engine::InputManager::getMouse().isPressed(HS_MOUSE_BUTTON_LEFT))
		{
			//if (canFire)
			//{
			//	RenderComponent* playerRenderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), RENDER));
			//	int playerModelIndex = playerRenderComponent->GetModelIndex();
			//	std::vector<vk::Model>& models = GetModels();
			//	models[playerModelIndex].animationQueue.push(models[playerModelIndex].animations[3]);
			//	models[playerModelIndex].blending = true;
			//	canFire = false;
			//	fireDelay = 1.0f;
			//}
		}
	}

	renderer.updateUniforms();
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
				//switch (serverCameraComponent.get)
				//{
				//case FPSTest::SCENE:
				//	camMode = FPSTest::PLAYER;

				//	break;
				//case FPSTest::PLAYER:
				//	camMode = FPSTest::SCENE;
				//	break;
				//default:
				//	camMode = FPSTest::SCENE;
				//	break;
				//}
				CameraComponent* cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
				cameraComponent->swapCameraMode();
			}
			return true;
		});
}

void FPSTest::DrawGUI()
{
	//FPSCameraComponent* cameraComponent = reinterpret_cast<FPSCameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));

	CameraComponent* cameraComponent = reinterpret_cast<CameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
	ImGui::Begin("Game:");
	ImGui::Text("GAME DEBUG");
	int cameraMode = cameraComponent->GetCamera()->camMode;
	ImGui::SliderInt("Camera Mode", &cameraMode, 0, 1);
	//ImGui::Text("Player Rotations:");
	//ImGui::SliderFloat("Cam: Pitch", &cameraComponent->GetCamera()->pitch, -60.0f, 60.0f);
	//ImGui::SliderFloat("Cam: Yaw", &cameraComponent->GetCamera()->yaw, -360.0f, 360.0f);
	//ImGui::InputFloat3("Cam Offset: ", &cameraOffset.x);
	//ImGui::Text("Yaw: %f, Pitch: %f", cameraComponent->GetCamera()->yaw, cameraComponent->GetCamera()->pitch);
	ImGui::Text("Game Settings:");
	//ImGui::InputFloat("Fire Delay", &fireDelay);
	//if (ImGui::Checkbox("Cam Can Rotate", &camCanRotate))
	//{
	//	//cameraComponent->setRotationEnabled(camCanRotate);
	//	int winX, winY;
	//	glfwGetFramebufferSize(this->GetContext().getGLFWWindow(), &winX, &winY);
	//	cameraComponent->GetCamera()->lastX = (float)winX / 2;
	//	cameraComponent->GetCamera()->lastY = (float)winY / 2;
	//	cameraComponent->GetCamera()->yaw = 0.0f;
	//	cameraComponent->GetCamera()->pitch = 0.0f;
	//};
	ImGui::End();
}

void FPSTest::initialiseModels()
{
	//// Here we would load all relevant glTF models and put them in the models vector
	//tinygltf::Model sponza = Engine::loadFromFile("Game/assets/Sponza/glTF/Sponza.gltf");
	//tinygltf::Model pistol = Engine::loadFromFile("Game/assets/Assets/guns/rifle/scene.gltf");
	////tinygltf::Model pistol = Engine::loadFromFile("Game/assets/Assets/guns/pistol1/scene.gltf");
	////tinygltf::Model pistol = Engine::loadFromFile("Game/assets/Assets/guns/pistolTest2/scene.gltf");

	//GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), sponza));
	//GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), pistol));

	tinygltf::Model sponza = Engine::loadFromFile("Game/assets/Sponza/glTF/Sponza.gltf");
	tinygltf::Model helmet = Engine::loadFromFile("Game/assets/DamagedHelmet.gltf");
	tinygltf::Model cube = Engine::loadFromFile("Game/assets/Cube.gltf");
	tinygltf::Model character = Engine::loadFromFile("Game/assets/Character/scene.gltf");
	tinygltf::Model pistol = Engine::loadFromFile("Game/assets/Assets/guns/pistol1/scene.gltf");
	tinygltf::Model target = Engine::loadFromFile("Game/assets/target/scene.gltf");
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), sponza));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), helmet));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), cube));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), character));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), pistol));
	GetModels().emplace_back(Engine::makeVulkanModel(this->GetContext(), target));
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

	// Map
	// ---- input actions ----- 
	// mapEntity = entityManager.AddEntity(types);
	// renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(mapEntity->GetEntityId(), RENDER));
	// renderComponent->SetModelIndex(0);
	// physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(mapEntity->GetEntityId(), PHYSICS));
	// physicsComponent->initComplexShape(physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], mapEntity->GetModelMatrix(), mapEntity->GetEntityId());

	// // Player 1
	// types = { CAMERA, RENDER, PHYSICS };
	// playerEntity = GetEntityManager().AddEntity(types);
	// playerEntity->SetScale(0.5f);
	// playerEntity->SetPosition(playerPos);




	// //playerEntity->SetRotation(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	// //glm::vec3 cameraPos = playerPos + glm::vec3(0.0f, 2.0f, 0.0f);

	// CameraComponent* cameraComponent = reinterpret_cast<FPSCameraComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
	// cameraComponent->SetCamera(&playerCam);
	// renderComponent = reinterpret_cast<RenderComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), RENDER));
	// renderComponent->SetModelIndex(1);
	// physicsComponent = reinterpret_cast<PhysicsComponent*>(GetEntityManager().GetComponentOfEntity(playerEntity->GetEntityId(), PHYSICS));
	// physicsComponent->init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[renderComponent->GetModelIndex()], playerEntity->GetModelMatrix(), playerEntity->GetEntityId());

	// //// Pistol
	// //types = { RENDER };
	// //entity = entityManager.AddEntity(types);
	// //entity->SetPosition(-1.0f, 1.0f, 0.0f);
	// //entity->SetRotation(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	// //entity->SetScale(0.5f);
	// //renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	// //renderComponent->SetModelIndex(4);

	// // ---- input actions ----- 
	std::vector<ComponentTypes> types = { RENDER, PHYSICS };
	mapEntity = entityManager.MakeNewEntity(types);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(mapEntity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(0);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(mapEntity->GetEntityId(), PHYSICS));
	physicsComponent->InitComplexShape(physicsWorld, PhysicsComponent::PhysicsType::STATIC, models[renderComponent->GetModelIndex()], mapEntity->GetModelMatrix(), mapEntity->GetEntityId());
	entityManager.AddSimulatedPhysicsEntity(mapEntity->GetEntityId());

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
	//entityManager.AddSimulatedPhysicsEntity(entity->GetEntityId());

	// Player 1
	types = { CAMERA, NETWORK, RENDER, PHYSICS };
	playerEntity = entityManager.MakeNewEntity(types);
	playerEntity->SetPosition(0.0f, 0.5f, 0.0f);
	//playerEntity->SetPosition(0.0f, 2.0f, 0.0f);
	//playerEntity->SetRotation(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	//playerEntity->SetRotation(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//playerEntity->SetRotation(90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
	//playerEntity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	//playerEntity->SetRotation(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	//glm::mat4 rZ = glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	//glm::mat4 rY = glm::rotate(glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	//playerEntity->SetRotation(rY * rZ);
	//playerEntity->SetRotation(rZ * rY);
	playerEntity->SetScale(30.0f);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(playerEntity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(3); // changed
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(playerEntity->GetEntityId(), PHYSICS));
	physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::CONTROLLER, models[renderComponent->GetModelIndex()], playerEntity->GetModelMatrix(), playerEntity->GetEntityId(), true, true);
	entityManager.AddSimulatedPhysicsEntity(playerEntity->GetEntityId());
	cameraComponent = reinterpret_cast<CameraComponent*>(entityManager.GetComponentOfEntity(playerEntity->GetEntityId(), CAMERA));
	cameraComponent->SetCamera(sceneCam);
	cameraComponent->playerEntity = playerEntity;
	networkComponent = reinterpret_cast<NetworkComponent*>(entityManager.GetComponentOfEntity(playerEntity->GetEntityId(), NETWORK));
	networkComponent->SetClientId(0);
	GetRenderer().attachCameraComponent(cameraComponent);

	//// Player 2
	//types = { RENDER };
	//entity = entityManager.MakeNewEntity(types);
	//entity->SetPosition(5.0f, 0.0f, -1.0f);
	//entity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
	//entity->SetScale(30.0f);
	//renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	//renderComponent->SetModelIndex(3);

	// Pistol
	types = { RENDER };
	entity = entityManager.MakeNewEntity(types);
	entity->SetPosition(-1.0f, 1.0f, 0.0f);
	entity->SetRotation(-90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	entity->SetScale(0.5f);
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(4);

	std::vector<int> entitiesWithNetworkComponent = entityManager.GetEntitiesWithComponent(NETWORK);
	for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
	{
		networkComponent = reinterpret_cast<NetworkComponent*>(entityManager.GetComponentOfEntity(entitiesWithNetworkComponent[i], NETWORK));
		if (networkComponent->GetClientId() == offlineClientId)
		{
			cameraComponent = reinterpret_cast<CameraComponent*>(entityManager.GetComponentOfEntity(entitiesWithNetworkComponent[i], CAMERA));
			GetRenderer().attachCameraComponent(cameraComponent);
		}
	}

	//GetEntityManager().ResetChanged();
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
	entity = entityManager.MakeNewEntity(types);
	entity->SetPosition(0.0f, 2.0f, 0.0f);
	entity->SetRotation(90.0f, glm::vec3(0.0f, 1.0f, 0.0f));
	entity->SetScale(0.2f);
	// configure physics component
	renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
	renderComponent->SetModelIndex(1);
	physicsComponent = reinterpret_cast<PhysicsComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
	physicsComponent->Init(physicsWorld, PhysicsComponent::PhysicsType::DYNAMIC, models[renderComponent->GetModelIndex()], entity->GetModelMatrix(), entity->GetEntityId(), false, false);
	entityManager.AddSimulatedPhysicsEntity(entity->GetEntityId());

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
	types = { CAMERA, NETWORK, RENDER, PHYSICS };
	for (int i = 0; i < maxClientsNum; i++)
	{
		entity = entityManager.MakeNewEntity(types);
		entity->SetPosition(-5.0f, 0.0f, -1.0f);
		entity->SetRotation(90.0f, glm::vec3(0.0f, 0.0f, 1.0f));
		entity->SetScale(30.0f);
		renderComponent = reinterpret_cast<RenderComponent*>(entityManager.GetComponentOfEntity(entity->GetEntityId(), RENDER));
		renderComponent->SetModelIndex(3);
		renderComponent->SetIsActive(0); // Players not renderable until client connection
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

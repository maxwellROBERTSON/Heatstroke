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
	/*PhysicsWorld p;
	p.init();*/

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
		loadOfflineEntities(registry, entityManager);
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

	// Create required objects for rendering
	std::map<std::string, Engine::vk::RenderPass> renderPasses;
	renderPasses.emplace("default", Engine::createRenderPass(*this->GetContext().window));

	std::map<std::string, Engine::vk::DescriptorSetLayout> descriptorLayouts;
	descriptorLayouts.emplace("sceneLayout", Engine::createSceneLayout(*this->GetContext().window));
	descriptorLayouts.emplace("materialLayout", Engine::createMaterialLayout(*this->GetContext().window));
	descriptorLayouts.emplace("materialInfoLayout", Engine::createSSBOLayout(*this->GetContext().window));
	descriptorLayouts.emplace("modelMatricesLayout", Engine::createDynamicUBOLayout(*this->GetContext().window));

	std::vector<VkDescriptorSetLayout> sceneDescriptorLayouts;
	sceneDescriptorLayouts.emplace_back(descriptorLayouts["sceneLayout"].handle);
	sceneDescriptorLayouts.emplace_back(descriptorLayouts["materialLayout"].handle);
	sceneDescriptorLayouts.emplace_back(descriptorLayouts["materialInfoLayout"].handle);
	sceneDescriptorLayouts.emplace_back(descriptorLayouts["modelMatricesLayout"].handle);

	std::map<std::string, Engine::vk::PipelineLayout> pipelineLayouts;
	pipelineLayouts.emplace("default", Engine::createPipelineLayout(*this->GetContext().window, sceneDescriptorLayouts, true));

	std::map<std::string, Engine::vk::Pipeline> pipelines;
	pipelines.emplace("default", Engine::createPipeline(*this->GetContext().window, renderPasses["default"].handle, pipelineLayouts["default"].handle));

	std::map<std::string, std::tuple<Engine::vk::Texture, Engine::vk::ImageView>> buffers;
	buffers.emplace("depthBuffer", Engine::createDepthBuffer(*this->GetContext().window, *this->GetContext().allocator));

	// Create basic swapchain framebuffers
	std::vector<Engine::vk::Framebuffer> framebuffers;
	Engine::createFramebuffers(*this->GetContext().window, framebuffers, renderPasses["default"].handle, std::get<1>(buffers["depthBuffer"]).handle);

	// Setup synchronisation
	std::size_t frameIndex = 0;
	std::vector<VkCommandBuffer> cmdBuffers;
	std::vector<Engine::vk::Fence> frameDone;
	std::vector<Engine::vk::Semaphore> imageAvailable, renderFinished;

	for (std::size_t i = 0; i < framebuffers.size(); i++) {
		cmdBuffers.emplace_back(Engine::createCommandBuffer(*this->GetContext().window));
		frameDone.emplace_back(Engine::createFence(*this->GetContext().window, VK_FENCE_CREATE_SIGNALED_BIT));
		imageAvailable.emplace_back(Engine::createSemaphore(*this->GetContext().window));
		renderFinished.emplace_back(Engine::createSemaphore(*this->GetContext().window));
	}

	// Create uniform buffers
	Engine::vk::Buffer sceneUBO = Engine::vk::createBuffer("sceneUBO", *this->GetContext().allocator, sizeof(glsl::SceneUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

	// Dynamic UBO's need to be correctly aligned (maybe move most of this out of the game loop method and put somewhere else)
	std::size_t uboAlignment = this->GetContext().window->device->minUBOAlignment;
	std::size_t dynamicAlignment = (sizeof(glm::mat4) + uboAlignment - 1) & ~(uboAlignment - 1);
	glsl::ModelMatricesUniform modelMatrices;
	Engine::vk::Buffer modelMatricesBuf = Engine::setupDynamicUBO(this->GetContext(), entityManager.GetComponentTypeSize<RenderComponent>(), dynamicAlignment, modelMatrices);
	vmaMapMemory(this->GetContext().allocator->allocator, modelMatricesBuf.allocation, &modelMatricesBuf.mapped);

	std::map<std::string, VkDescriptorSet> descriptorSets;
	descriptorSets.emplace("sceneDescriptors", Engine::createSceneDescriptor(*this->GetContext().window, descriptorLayouts["sceneLayout"].handle, sceneUBO.buffer));
	descriptorSets.emplace("modelMatricesDescriptor", Engine::createModelMatricesDescriptor(*this->GetContext().window, descriptorLayouts["modelMatricesLayout"].handle, modelMatricesBuf.buffer, dynamicAlignment));

	// For each model create its descriptor sets
	for (Engine::vk::Model& model : models) {
		model.createDescriptorSets(this->GetContext(), descriptorLayouts["materialLayout"].handle, descriptorLayouts["materialInfoLayout"].handle);
	}

	auto previous = std::chrono::steady_clock::now();

	while (!glfwWindowShouldClose(this->GetContext().getGLFWWindow())) {
		glfwPollEvents();

		if (recreateSwapchain) {
			vkDeviceWaitIdle(this->GetContext().window->device->device);

			const auto changes = Engine::recreateSwapchain(*this->GetContext().window);

			if (changes.changedFormat)
				Engine::recreateFormatDependents(*this->GetContext().window, renderPasses);

			if (changes.changedSize)
				Engine::recreateSizeDependents(this->GetContext(), renderPasses, pipelineLayouts, buffers, pipelines);

			Engine::recreateOthers(*this->GetContext().window, renderPasses, buffers, framebuffers, descriptorSets);

			std::fprintf(stdout, "size: %d - format: %d\n", changes.changedSize, changes.changedFormat);

			recreateSwapchain = false;
			continue;
		}

		frameIndex++;
		frameIndex %= cmdBuffers.size();

		Engine::waitForFences(*this->GetContext().window, frameDone, frameIndex);

		std::uint32_t imageIndex = 0;
		if (Engine::acquireNextSwapchainImage(*this->GetContext().window, imageAvailable, frameIndex, imageIndex)) {
			recreateSwapchain = true;

			--frameIndex;
			frameIndex %= cmdBuffers.size();

			continue;
		}

		Engine::resetFences(*this->GetContext().window, frameDone, frameIndex);

		assert(std::size_t(frameIndex) < cmdBuffers.size());
		assert(std::size_t(imageIndex) < framebuffers.size());

		// Calculate time delta
		const auto now = std::chrono::steady_clock::now();
		const auto timeDelta = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - previous).count();
		previous = now;

		camera->updateCamera(this->GetContext().getGLFWWindow(), timeDelta);

		glsl::SceneUniform sceneUniform{};
		updateSceneUniform(sceneUniform, *camera, this->GetContext().window->swapchainExtent.width, this->GetContext().window->swapchainExtent.height);
		updateModelMatrices(this->GetContext(), modelMatrices, modelMatricesBuf, entityManager, dynamicAlignment);

		Engine::renderModels(
			entityManager,
			models,
			cmdBuffers[frameIndex],
			renderPasses["default"].handle,
			framebuffers[imageIndex].handle,
			pipelines["default"].handle,
			this->GetContext().window->swapchainExtent,
			sceneUBO.buffer,
			sceneUniform,
			pipelineLayouts["default"].handle,
			descriptorSets["sceneDescriptors"],
			descriptorSets["modelMatricesDescriptor"],
			dynamicAlignment
		);

		assert(std::size_t(frameIndex) < renderFinished.size());

		const VkResult result = Engine::submitAndPresent(*this->GetContext().window, cmdBuffers, frameDone, imageAvailable, renderFinished, frameIndex, imageIndex);
		if (VK_SUBOPTIMAL_KHR == result || VK_ERROR_OUT_OF_DATE_KHR == result)
			recreateSwapchain = true;
		else if (VK_SUCCESS != result)
			throw Utils::Error("Unable to present swapchain image %u\n vkQueuePresentKHR() returned %s", imageIndex, Utils::toString(result).c_str());
	}

	vkDeviceWaitIdle(this->GetContext().window->device->device);

	vmaUnmapMemory(this->GetContext().allocator->allocator, modelMatricesBuf.allocation);
}

void updateSceneUniform(glsl::SceneUniform& aScene, Camera& camera, std::uint32_t aFramebufferWidth, std::uint32_t aFramebufferHeight) {
	const float aspectRatio = aFramebufferWidth / float(aFramebufferHeight);

	aScene.projection = glm::perspective(glm::radians(camera.fov), aspectRatio, camera.nearPlane, camera.farPlane);
	aScene.projection[1][1] *= -1.0f;
	aScene.view = glm::lookAt(camera.position, camera.position + camera.frontDirection, glm::vec3(0.0f, 1.0f, 0.0f));
	aScene.position = glm::vec4(camera.position, 1.0f);
}

void updateModelMatrices(const Engine::VulkanContext& aContext, glsl::ModelMatricesUniform& aModelMatrices, Engine::vk::Buffer& aBuffer, EntityManager& entityManager, std::size_t dynamicAlignment) {

	std::vector<int> aModels = entityManager.GetEntitiesWithComponent<RenderComponent>();
	for (std::size_t i = 0; i < aModels.size(); i++) {
		glm::mat4* modelMatrix = (glm::mat4*)((std::uint64_t)aModelMatrices.model + (i * dynamicAlignment));

		// This will need to be changed to get a 'parent' model matrix, not
		// just the first node's model matrix.
		*modelMatrix = entityManager.GetEntity(aModels[i])->GetModelMatrix();
	}

	int size = entityManager.GetComponentTypeSize<RenderComponent>() * dynamicAlignment;

	std::memcpy(aBuffer.mapped, aModelMatrices.model, size);

	vmaFlushAllocation(aContext.allocator->allocator, aBuffer.allocation, 0, size);
}

void loadOfflineEntities(ComponentTypeRegistry& registry, EntityManager& entityManager)
{
	// Pointers
	Entity* entity;
	RenderComponent* renderComponent;
	PhysicsComponent* physicsComponent;
	CameraComponent* cameraComponent;
	NetworkComponent* networkComponent;

	// Map
	entity = entityManager.AddEntity<RenderComponent, PhysicsComponent>();
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
	helmetTransform = glm::scale(helmetTransform, glm::vec3(0.2f, 0.2f, 0.2f));
	entity->SetModelMatrix(helmetTransform);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(1);

	// Cube
	entity = entityManager.AddEntity<RenderComponent, PhysicsComponent>();
	glm::mat4 cubeTransform(1.0f);
	cubeTransform = glm::translate(cubeTransform, glm::vec3(0.3f, 1.0f, -1.0f));
	cubeTransform = glm::scale(cubeTransform, glm::vec3(0.2f, 0.2f, 0.2f));
	entity->SetModelMatrix(cubeTransform);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(2);

	// Player 1
	entity = entityManager.AddEntity<RenderComponent, PhysicsComponent, CameraComponent, NetworkComponent>();
	glm::mat4 player1Transform(1.0f);
	player1Transform = glm::translate(player1Transform, glm::vec3(-5.0f, 1.0f, -1.0f));
	player1Transform = glm::rotate(player1Transform, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	player1Transform = glm::scale(player1Transform, glm::vec3(1.0f, 1.0f, 1.0f));
	entity->SetModelMatrix(player1Transform);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(3);
	physicsComponent = entityManager.GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->SetIsPerson(true);
	cameraComponent = entityManager.GetEntityComponent<CameraComponent>(entity->GetEntityId());
	cameraComponent->SetCamera(Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f)));
	networkComponent = entityManager.GetEntityComponent<NetworkComponent>(entity->GetEntityId());
	networkComponent->SetClientId(0);

	// Player 2
	entity = entityManager.AddEntity<RenderComponent, PhysicsComponent, CameraComponent, NetworkComponent>();
	glm::mat4 player2Transform(1.0f);
	player2Transform = glm::translate(player2Transform, glm::vec3(5.0f, 1.0f, -1.0f));
	player2Transform = glm::rotate(player2Transform, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	player2Transform = glm::scale(player2Transform, glm::vec3(1.0f, 1.0f, 1.0f));
	entity->SetModelMatrix(player2Transform);
	renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
	renderComponent->SetModelIndex(3);
	physicsComponent = entityManager.GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
	physicsComponent->SetIsPerson(true);
}
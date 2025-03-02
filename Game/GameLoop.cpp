#include "GameLoop.hpp"

#include <cstdio>
#include <memory>
#include <chrono>
#include <string>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <yojimbo.h>

#include "../Engine/Network/Client/GameClient.hpp"

#include "../Engine/ECS/ComponentTypeRegistry.hpp"
#include "../Engine/ECS/EntityManager.hpp"
#include "../Engine/ECS/RenderComponent.hpp"
#include "../Engine/ECS/PhysicsComponent.hpp"
#include "../Engine/ECS/CameraComponent.hpp"
#include "../Engine/ECS/NetworkComponent.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "Uniforms.hpp"

Engine::VulkanContext vkContext;
Camera camera;
bool recreateSwapchain = false;

// Local clientId
int clientId = 0;

// Game mode
bool online = false;
bool switchMode = true;

void initialiseGame(ComponentTypeRegistry& registry, EntityManager& entityManager) {
    // Register component types
    registry.RegisterComponentTypes<
        RenderComponent,
        PhysicsComponent,
        CameraComponent,
        NetworkComponent
    >();

	// Make component arrays
	RenderComponent* renderComponents = new RenderComponent[1];
    PhysicsComponent* physicsComponents = new PhysicsComponent[1];
	CameraComponent* cameraComponents = new CameraComponent[1];
	NetworkComponent* networkComponents = new NetworkComponent[1];

    // Make vector of pointers to each component type's data
    std::vector<std::pair<void*, int>> componentTypePointers;

    // Push back the address of the component vectors
    componentTypePointers.push_back(std::make_pair(reinterpret_cast<void*>(renderComponents), 0));
	componentTypePointers.push_back(std::make_pair(reinterpret_cast<void*>(physicsComponents), 0));
	componentTypePointers.push_back(std::make_pair(reinterpret_cast<void*>(cameraComponents), 0));
	componentTypePointers.push_back(std::make_pair(reinterpret_cast<void*>(networkComponents), 0));

	// Add component pointers to the entity manager
	entityManager.SetComponentTypesPointers(componentTypePointers);

	// Allocate entities with type vectors
	entityManager.SetEntitiesWithType();

#ifdef CLIENT
    vkContext.window = Engine::initialiseVulkan();
    vkContext.allocator = Engine::createVulkanAllocator(*vkContext.window.get());
    Engine::registerCallbacks(vkContext.getGLFWWindow());
#endif

    // Move the starting camera position to the left and look in the +x direction
    camera = Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
}

void initialiseModels(std::vector<Engine::vk::Model>& models) {
    // Here we would load all relevant glTF models and put them in the models vector
    tinygltf::Model sponza = Engine::loadFromFile("Game/assets/Sponza/glTF/Sponza.gltf");
    tinygltf::Model helmet = Engine::loadFromFile("Game/assets/DamagedHelmet.gltf");
    tinygltf::Model cube = Engine::loadFromFile("Game/assets/Cube.gltf");
    tinygltf::Model character = Engine::loadFromFile("Game/assets/Character/scene.gltf");
    models.emplace_back(Engine::makeVulkanModel(vkContext, sponza));
    models.emplace_back(Engine::makeVulkanModel(vkContext, helmet));
    models.emplace_back(Engine::makeVulkanModel(vkContext, cube));
	models.emplace_back(Engine::makeVulkanModel(vkContext, character));
}

void runGameLoop(std::vector<Engine::vk::Model>& models, ComponentTypeRegistry& registry, EntityManager& entityManager) {
    // Offline mode only for now
	loadOfflineEntities(registry, entityManager);

    // Create required objects for rendering
    std::map<std::string, Engine::vk::RenderPass> renderPasses;
    renderPasses.emplace("default", Engine::createRenderPass(*vkContext.window));

    std::map<std::string, Engine::vk::DescriptorSetLayout> descriptorLayouts;
    descriptorLayouts.emplace("sceneLayout", Engine::createSceneLayout(*vkContext.window));
    descriptorLayouts.emplace("materialLayout", Engine::createMaterialLayout(*vkContext.window));
    descriptorLayouts.emplace("materialInfoLayout", Engine::createSSBOLayout(*vkContext.window));
    descriptorLayouts.emplace("modelMatricesLayout", Engine::createDynamicUBOLayout(*vkContext.window));

    std::vector<VkDescriptorSetLayout> sceneDescriptorLayouts;
    sceneDescriptorLayouts.emplace_back(descriptorLayouts["sceneLayout"].handle);
    sceneDescriptorLayouts.emplace_back(descriptorLayouts["materialLayout"].handle);
    sceneDescriptorLayouts.emplace_back(descriptorLayouts["materialInfoLayout"].handle);
    sceneDescriptorLayouts.emplace_back(descriptorLayouts["modelMatricesLayout"].handle);

    std::map<std::string, Engine::vk::PipelineLayout> pipelineLayouts;
    pipelineLayouts.emplace("default", Engine::createPipelineLayout(*vkContext.window, sceneDescriptorLayouts, true));

    std::map<std::string, Engine::vk::Pipeline> pipelines;
    pipelines.emplace("default", Engine::createPipeline(*vkContext.window, renderPasses["default"].handle, pipelineLayouts["default"].handle));

    std::map<std::string, std::tuple<Engine::vk::Texture, Engine::vk::ImageView>> buffers;
    buffers.emplace("depthBuffer", Engine::createDepthBuffer(*vkContext.window, *vkContext.allocator));

    // Create basic swapchain framebuffers
    std::vector<Engine::vk::Framebuffer> framebuffers;
    Engine::createFramebuffers(*vkContext.window, framebuffers, renderPasses["default"].handle, std::get<1>(buffers["depthBuffer"]).handle);

    // Setup synchronisation
    std::size_t frameIndex = 0;
    std::vector<VkCommandBuffer> cmdBuffers;
    std::vector<Engine::vk::Fence> frameDone;
    std::vector<Engine::vk::Semaphore> imageAvailable, renderFinished;

    for (std::size_t i = 0; i < framebuffers.size(); i++) {
        cmdBuffers.emplace_back(Engine::createCommandBuffer(*vkContext.window));
        frameDone.emplace_back(Engine::createFence(*vkContext.window, VK_FENCE_CREATE_SIGNALED_BIT));
        imageAvailable.emplace_back(Engine::createSemaphore(*vkContext.window));
        renderFinished.emplace_back(Engine::createSemaphore(*vkContext.window));
    }

    // Create uniform buffers
    Engine::vk::Buffer sceneUBO = Engine::vk::createBuffer("sceneUBO", *vkContext.allocator, sizeof(glsl::SceneUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
    
    // Dynamic UBO's need to be correctly aligned (maybe move most of this out of the game loop method and put somewhere else)
    std::size_t uboAlignment = vkContext.window->device->minUBOAlignment;
    std::size_t dynamicAlignment = (sizeof(glm::mat4) + uboAlignment - 1) & ~(uboAlignment - 1);
    glsl::ModelMatricesUniform modelMatrices;
    Engine::vk::Buffer modelMatricesBuf = Engine::setupDynamicUBO(vkContext, entityManager.GetComponentTypeSize<RenderComponent>(), dynamicAlignment, modelMatrices);
    vmaMapMemory(vkContext.allocator->allocator, modelMatricesBuf.allocation, &modelMatricesBuf.mapped);

    std::map<std::string, VkDescriptorSet> descriptorSets;
    descriptorSets.emplace("sceneDescriptors", Engine::createSceneDescriptor(*vkContext.window, descriptorLayouts["sceneLayout"].handle, sceneUBO.buffer));
    descriptorSets.emplace("modelMatricesDescriptor", Engine::createModelMatricesDescriptor(*vkContext.window, descriptorLayouts["modelMatricesLayout"].handle, modelMatricesBuf.buffer, dynamicAlignment));

    // For each model create its descriptor sets
    for (Engine::vk::Model& model : models) {
        model.createDescriptorSets(vkContext, descriptorLayouts["materialLayout"].handle, descriptorLayouts["materialInfoLayout"].handle);
    }

    auto previous = std::chrono::steady_clock::now();

    while (!glfwWindowShouldClose(vkContext.getGLFWWindow())) {
        glfwPollEvents();

        if (recreateSwapchain) {
            vkDeviceWaitIdle(vkContext.window->device->device);

            const auto changes = Engine::recreateSwapchain(*vkContext.window);

            if (changes.changedFormat)
                Engine::recreateFormatDependents(*vkContext.window, renderPasses);

            if (changes.changedSize)
                Engine::recreateSizeDependents(vkContext, renderPasses, pipelineLayouts, buffers, pipelines);

            Engine::recreateOthers(*vkContext.window, renderPasses, buffers, framebuffers, descriptorSets);

            std::fprintf(stdout, "size: %d - format: %d\n", changes.changedSize, changes.changedFormat);

            recreateSwapchain = false;
            continue;
        }

        frameIndex++;
        frameIndex %= cmdBuffers.size();

        Engine::waitForFences(*vkContext.window, frameDone, frameIndex);

        std::uint32_t imageIndex = 0;
        if (Engine::acquireNextSwapchainImage(*vkContext.window, imageAvailable, frameIndex, imageIndex)) {
            recreateSwapchain = true;

            --frameIndex;
            frameIndex %= cmdBuffers.size();

            continue;
        }

        Engine::resetFences(*vkContext.window, frameDone, frameIndex);

        assert(std::size_t(frameIndex) < cmdBuffers.size());
        assert(std::size_t(imageIndex) < framebuffers.size());

        // Calculate time delta
        const auto now = std::chrono::steady_clock::now();
        const auto timeDelta = std::chrono::duration_cast<std::chrono::duration<float, std::ratio<1>>>(now - previous).count();
        previous = now;

        camera.updateCamera(vkContext.getGLFWWindow(), timeDelta);
        
        glsl::SceneUniform sceneUniform{};
        updateSceneUniform(sceneUniform, camera, vkContext.window->swapchainExtent.width, vkContext.window->swapchainExtent.height);
        updateModelMatrices(vkContext, modelMatrices, modelMatricesBuf, entityManager, dynamicAlignment);

        Engine::renderModels(
			entityManager,
            models,
            cmdBuffers[frameIndex], 
            renderPasses["default"].handle,
            framebuffers[imageIndex].handle,
            pipelines["default"].handle,
            vkContext.window->swapchainExtent,
            sceneUBO.buffer,
            sceneUniform,
            pipelineLayouts["default"].handle,
            descriptorSets["sceneDescriptors"],
            descriptorSets["modelMatricesDescriptor"],
            dynamicAlignment
        );

        assert(std::size_t(frameIndex) < renderFinished.size());

        const VkResult result = Engine::submitAndPresent(*vkContext.window, cmdBuffers, frameDone, imageAvailable, renderFinished, frameIndex, imageIndex);
        if (VK_SUBOPTIMAL_KHR == result || VK_ERROR_OUT_OF_DATE_KHR == result)
            recreateSwapchain = true;
        else if (VK_SUCCESS != result)
            throw Utils::Error("Unable to present swapchain image %u\n vkQueuePresentKHR() returned %s", imageIndex, Utils::toString(result).c_str());
    }

    vkDeviceWaitIdle(vkContext.window->device->device);

    vmaUnmapMemory(vkContext.allocator->allocator, modelMatricesBuf.allocation);
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
    cameraComponent->SetCamera(camera);
    networkComponent = entityManager.GetEntityComponent<NetworkComponent>(entity->GetEntityId());
    networkComponent->SetClientId(clientId);

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
#include <cstdio>
#include <memory>
#include <chrono>
#include <string>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <yojimbo.h>

#include "../Engine/vulkan/VulkanContext.hpp"
#include "../Engine/vulkan/VulkanAllocator.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"
#include "../Engine/glfw/Callbacks.hpp"
#include "../Engine/vulkan/VulkanWindow.hpp"
#include "../Engine/vulkan/Renderer.hpp"
#include "../Engine/vulkan/PipelineCreation.hpp"
#include "../Engine/vulkan/objects/Buffer.hpp"
#include "../Engine/gltf/glTF.hpp"

#include "../Engine/ECS/ComponentTypeRegistry.hpp"
#include "../Engine/ECS/EntityManager.hpp"
#include "../Engine/ECS/RenderComponent.hpp"
#include "../Engine/ECS/PhysicsComponent.hpp"
#include "../Engine/ECS/CameraComponent.hpp"
#include "../Engine/ECS/NetworkComponent.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "Uniforms.hpp"

#include "Camera.hpp"

namespace {
    void initialiseGame();

    void runGameLoop();

    void updateSceneUniform(glsl::SceneUniform& aScene, Camera& camera, std::uint32_t aFramebufferWidth, std::uint32_t aFramebufferHeight);

    void LoadOfflineEntities();

    Engine::VulkanContext vkContext;

    Camera camera;
    bool recreateSwapchain = false;
    std::vector<Engine::vk::Model> models;

    // Make a registry of component types
    ComponentTypeRegistry registry = ComponentTypeRegistry::Get();

    // Make an entity manager
    EntityManager entityManager = EntityManager(&registry);

    // Local clientId
    int clientId = 0;

    // Game mode
	bool online = false;
	bool switchMode = true;
}

namespace {
    void initialiseGame() {
        // Register component types
        registry.RegisterComponentTypes<
            RenderComponent,
            PhysicsComponent,
            CameraComponent,
            NetworkComponent
        >(); //TestUserComponent>();

		// Make component arrays
		RenderComponent* renderComponents = new RenderComponent[1];
        PhysicsComponent* physicsComponents = new PhysicsComponent[1];
		CameraComponent* cameraComponents = new CameraComponent[1];
		NetworkComponent* networkComponents = new NetworkComponent[1];
		//TestUserComponent* testUserComponents = new TestUserComponent[1];

        // Make vector of pointers to each component type's data
		std::vector<std::pair<void*, int>> componentTypePointers;

        // Push back the address of the component vectors
        componentTypePointers.push_back(std::make_pair(reinterpret_cast<void*>(renderComponents), 0));
		componentTypePointers.push_back(std::make_pair(reinterpret_cast<void*>(physicsComponents), 0));
		componentTypePointers.push_back(std::make_pair(reinterpret_cast<void*>(cameraComponents), 0));
		componentTypePointers.push_back(std::make_pair(reinterpret_cast<void*>(networkComponents), 0));
        //componentTypePointers.push_back(std::make_pair(reinterpret_cast<void*>(testUserComponents), 0));

		// Add component pointers to the entity manager
		entityManager.SetComponentTypesPointers(componentTypePointers);

#ifdef CLIENT
        vkContext.window = Engine::initialiseVulkan();
        vkContext.allocator = Engine::createVulkanAllocator(*vkContext.window.get());
        Engine::registerCallbacks(vkContext.getGLFWWindow());

        // Here we would load all relevant glTF models and put them in the models vector
        tinygltf::Model tinygltfmodel = Engine::loadFromFile("Game/assets/DamagedHelmet.gltf");
        models.emplace_back(Engine::makeVulkanModel(vkContext, tinygltfmodel));
#endif

        camera = Camera(100.0f, 0.01f, 256.0f, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    void runGameLoop() {
         // Create required objects for rendering
        std::map<std::string, Engine::vk::RenderPass> renderPasses;
        renderPasses.emplace("default", Engine::createRenderPass(*vkContext.window));

        std::map<std::string, Engine::vk::DescriptorSetLayout> descriptorLayouts;
        descriptorLayouts.emplace("sceneLayout", Engine::createSceneLayout(*vkContext.window));
        descriptorLayouts.emplace("materialLayout", Engine::createMaterialLayout(*vkContext.window));

        std::vector<VkDescriptorSetLayout> sceneDescriptorLayouts;
        sceneDescriptorLayouts.emplace_back(descriptorLayouts["sceneLayout"].handle);
        sceneDescriptorLayouts.emplace_back(descriptorLayouts["materialLayout"].handle);

        std::map<std::string, Engine::vk::PipelineLayout> pipelineLayouts;
        pipelineLayouts.emplace("default", Engine::createPipelineLayout(*vkContext.window, sceneDescriptorLayouts));

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
        Engine::vk::Buffer sceneUBO = Engine::vk::createBuffer(*vkContext.allocator, sizeof(glsl::SceneUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

        std::map<std::string, VkDescriptorSet> descriptorSets;
        VkDescriptorSet sceneDescriptors = Engine::allocateDescriptorSet(*vkContext.window, vkContext.window->device->dPool, descriptorLayouts["sceneLayout"].handle);
        {
            VkWriteDescriptorSet desc[1]{};

            VkDescriptorBufferInfo sceneUboInfo{};
            sceneUboInfo.buffer = sceneUBO.buffer;
            sceneUboInfo.range = VK_WHOLE_SIZE;

            desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            desc[0].dstSet = sceneDescriptors;
            desc[0].dstBinding = 0;
            desc[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
            desc[0].descriptorCount = 1;
            desc[0].pBufferInfo = &sceneUboInfo;

            constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
            vkUpdateDescriptorSets(vkContext.window->device->device, numSets, desc, 0, nullptr);
        }
        descriptorSets.emplace("sceneDescriptors", sceneDescriptors);

        // We will need to change the following to create all the samplers, descriptors etc for each model in the models vector
        // rather than just doing it for the first one, I have it this way for now just to get something on screen.

        // Create all texture samplers
        std::vector<Engine::vk::Sampler> samplers;
        Engine::createTextureSamplers(*vkContext.window, models[0], samplers);

        VkDescriptorSet materialDescriptors = Engine::allocateDescriptorSet(*vkContext.window, vkContext.window->device->dPool, descriptorLayouts["materialLayout"].handle);
        {
            VkWriteDescriptorSet desc[5]{};

            VkDescriptorImageInfo baseColourInfo{};
            baseColourInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            baseColourInfo.imageView = models[0].imageViews[models[0].materials[0].baseColourTextureIndex].handle;
            baseColourInfo.sampler = samplers[models[0].textures[models[0].materials[0].baseColourTextureIndex].sampler].handle; // This could definitely be done better

            desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            desc[0].dstSet = materialDescriptors;
            desc[0].dstBinding = 0;
            desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc[0].descriptorCount = 1;
            desc[0].pImageInfo = &baseColourInfo;

            VkDescriptorImageInfo metallicRoughnessInfo{};
            metallicRoughnessInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            metallicRoughnessInfo.imageView = models[0].imageViews[models[0].materials[0].metallicRoughnessTextureIndex].handle;
            metallicRoughnessInfo.sampler = samplers[models[0].textures[models[0].materials[0].metallicRoughnessTextureIndex].sampler].handle;

            desc[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            desc[1].dstSet = materialDescriptors;
            desc[1].dstBinding = 1;
            desc[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc[1].descriptorCount = 1;
            desc[1].pImageInfo = &metallicRoughnessInfo;

            VkDescriptorImageInfo emissiveInfo{};
            emissiveInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            emissiveInfo.imageView = models[0].imageViews[models[0].materials[0].emissiveTextureIndex].handle;
            emissiveInfo.sampler = samplers[models[0].textures[models[0].materials[0].emissiveTextureIndex].sampler].handle;

            desc[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            desc[2].dstSet = materialDescriptors;
            desc[2].dstBinding = 2;
            desc[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc[2].descriptorCount = 1;
            desc[2].pImageInfo = &emissiveInfo;

            VkDescriptorImageInfo occlusionInfo{};
            occlusionInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            occlusionInfo.imageView = models[0].imageViews[models[0].materials[0].occlusionTextureIndex].handle;
            occlusionInfo.sampler = samplers[models[0].textures[models[0].materials[0].occlusionTextureIndex].sampler].handle;

            desc[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            desc[3].dstSet = materialDescriptors;
            desc[3].dstBinding = 3;
            desc[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc[3].descriptorCount = 1;
            desc[3].pImageInfo = &occlusionInfo;

            VkDescriptorImageInfo normalMapInfo{};
            normalMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            normalMapInfo.imageView = models[0].imageViews[models[0].materials[0].normalTextureIndex].handle;
            normalMapInfo.sampler = samplers[models[0].textures[models[0].materials[0].normalTextureIndex].sampler].handle;

            desc[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            desc[4].dstSet = materialDescriptors;
            desc[4].dstBinding = 4;
            desc[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc[4].descriptorCount = 1;
            desc[4].pImageInfo = &normalMapInfo;

            constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
            vkUpdateDescriptorSets(vkContext.window->device->device, numSets, desc, 0, nullptr);
        }
        descriptorSets.emplace("materialDescriptors", materialDescriptors);

        auto previous = std::chrono::steady_clock::now();

        while (!glfwWindowShouldClose(vkContext.getGLFWWindow())) {
            glfwPollEvents();

			if (!online && switchMode) {
				LoadOfflineEntities();
				switchMode = false;
			}

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

            Engine::renderModel(
                models[0],
                cmdBuffers[frameIndex],
                renderPasses["default"].handle,
                framebuffers[imageIndex].handle,
                pipelines["default"].handle,
                vkContext.window->swapchainExtent,
                sceneUBO.buffer,
                sceneUniform,
                pipelineLayouts["default"].handle,
                sceneDescriptors,
                materialDescriptors
            );

            assert(std::size_t(frameIndex) < renderFinished.size());

            const VkResult result = Engine::submitAndPresent(*vkContext.window, cmdBuffers, frameDone, imageAvailable, renderFinished, frameIndex, imageIndex);
            if (VK_SUBOPTIMAL_KHR == result || VK_ERROR_OUT_OF_DATE_KHR == result)
                recreateSwapchain = true;
            else if (VK_SUCCESS != result)
                throw Utils::Error("Unable to present swapchain image %u\n vkQueuePresentKHR() returned %s", imageIndex, Utils::toString(result).c_str());
        }

        vkDeviceWaitIdle(vkContext.window->device->device);
        vkContext.allocator.reset();
        vkContext.window.reset();

	}

    void updateSceneUniform(glsl::SceneUniform& aScene, Camera& camera, std::uint32_t aFramebufferWidth, std::uint32_t aFramebufferHeight) {
        const float aspectRatio = float(aFramebufferWidth / aFramebufferHeight);

        aScene.projection = glm::perspective(glm::radians(camera.fov), aspectRatio, camera.nearPlane, camera.farPlane);
        aScene.projection[1][1] *= -1.0f;
        aScene.view = glm::lookAt(camera.position, camera.position + camera.frontDirection, glm::vec3(0.0f, 1.0f, 0.0f));
        //aScene.model = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        aScene.position = glm::vec4(camera.position, 1.0f);
    }

    void LoadOfflineEntities()
    {
		// Pointers
        Entity* entity;
        RenderComponent* renderComponent;
		PhysicsComponent* physicsComponent;
		CameraComponent* cameraComponent;
		NetworkComponent* networkComponent;

        // Player
        entity = entityManager.AddEntity<RenderComponent, PhysicsComponent, CameraComponent, NetworkComponent>();
		renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
		renderComponent->SetModelIndex(0);
		physicsComponent = entityManager.GetEntityComponent<PhysicsComponent>(entity->GetEntityId());
		physicsComponent->SetIsPerson(true);
		cameraComponent = entityManager.GetEntityComponent<CameraComponent>(entity->GetEntityId());
		cameraComponent->SetCamera(camera);
		networkComponent = entityManager.GetEntityComponent<NetworkComponent>(entity->GetEntityId());
		networkComponent->SetClientId(clientId);
        // Map
		entity = entityManager.AddEntity<RenderComponent, PhysicsComponent>();
        renderComponent = entityManager.GetEntityComponent<RenderComponent>(entity->GetEntityId());
        renderComponent->SetModelIndex(0);
    }
}
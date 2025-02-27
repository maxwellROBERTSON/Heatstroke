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

#include "../Engine/Network/Client/GameClient.hpp"

#include "../Engine/ECS/ComponentTypeRegistry.hpp"
#include "../Engine/ECS/EntityManager.hpp"
#include "../Engine/ECS/RenderComponent.hpp"
#include "UserComponents/TestUserComponent.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "Uniforms.hpp"

#include "Camera.hpp"

namespace {
    void initialiseGame();

    void runGameLoop();

    void updateSceneUniform(glsl::SceneUniform& aScene, Camera& camera, std::uint32_t aFramebufferWidth, std::uint32_t aFramebufferHeight);
    void updateModelMatrices(const Engine::VulkanContext& aContext, glsl::ModelMatricesUniform& aModelMatrices, Engine::vk::Buffer& aBuffer, std::vector<Engine::vk::Model>& aModels, std::size_t dynamicAlignment);

    Engine::VulkanContext vkContext;

    Camera camera;
    bool recreateSwapchain = false;
    std::vector<Engine::vk::Model> models;
}

namespace {
    void initialiseGame() {
        // Make a registry of component types
        ComponentTypeRegistry registry = ComponentTypeRegistry::Get();

        // Make an entity manager
		EntityManager entityManager = EntityManager(&registry);

		// Register component types
		registry.RegisterComponentTypes<RenderComponent, TestUserComponent>();

		// Make component arrays
		RenderComponent* renderComponents = new RenderComponent[1];
		TestUserComponent* testUserComponents = new TestUserComponent[1];

        // Make vector of pointers to each component type's data
		std::vector<std::pair<void*, int>> componentTypePointers;

        // Push back the address of the component vectors
        componentTypePointers.push_back(std::make_pair(reinterpret_cast<void*>(renderComponents), 0));
        componentTypePointers.push_back(std::make_pair(reinterpret_cast<void*>(testUserComponents), 0));

		// Add component pointers to the entity manager
		entityManager.AddComponentTypesPointers(componentTypePointers);

        // Add entities
        entityManager.AddEntity<RenderComponent>();
        entityManager.AddEntity<TestUserComponent>();
        entityManager.AddEntity<RenderComponent, TestUserComponent>();

#ifdef CLIENT
        vkContext.window = Engine::initialiseVulkan();
        vkContext.allocator = Engine::createVulkanAllocator(*vkContext.window.get());
        Engine::registerCallbacks(vkContext.getGLFWWindow());

        // Here we would load all relevant glTF models and put them in the models vector
        tinygltf::Model sponza = Engine::loadFromFile("assets/Sponza/glTF/Sponza.gltf");
        tinygltf::Model helmet = Engine::loadFromFile("assets/DamagedHelmet.gltf");
        tinygltf::Model cube = Engine::loadFromFile("assets/Cube.gltf");
        models.emplace_back(Engine::makeVulkanModel(vkContext, sponza));
        models.emplace_back(Engine::makeVulkanModel(vkContext, helmet));
        models.emplace_back(Engine::makeVulkanModel(vkContext, cube));

        // Move the starting camera position to the left and look in the +x direction
        camera = Camera(100.0f, 0.01f, 256.0f, glm::vec3(-3.0f, 2.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    void runGameLoop() {
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
        Engine::vk::Buffer modelMatricesBuf = Engine::setupDynamicUBO(vkContext, models.size(), dynamicAlignment, modelMatrices);
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

            // Set up the transform matrix for the helmet
            glm::mat4 transform(1.0f);
            transform = glm::translate(transform, glm::vec3(0.0f, 2.0f, 0.0f));
            transform = glm::rotate(transform, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            transform = glm::scale(transform, glm::vec3(0.5f, 0.5f, 0.5f));

            // Transform the helmet (this will need to be changed as to not refer to the first node at some point)
            models[1].nodes[0]->setPostTransform(transform);

            glm::mat4 cubeTransform(1.0f);
            cubeTransform = glm::translate(cubeTransform, glm::vec3(0.0f, 1.0f, -1.0f));

            models[2].nodes[0]->setPostTransform(cubeTransform);
            
            glsl::SceneUniform sceneUniform{};
            updateSceneUniform(sceneUniform, camera, vkContext.window->swapchainExtent.width, vkContext.window->swapchainExtent.height);
            updateModelMatrices(vkContext, modelMatrices, modelMatricesBuf, models, dynamicAlignment);


            Engine::renderModels(
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

    void updateModelMatrices(const Engine::VulkanContext& aContext, glsl::ModelMatricesUniform& aModelMatrices, Engine::vk::Buffer& aBuffer, std::vector<Engine::vk::Model>& aModels, std::size_t dynamicAlignment) {
    
        for (std::size_t i = 0; i < aModels.size(); i++) {
            glm::mat4* modelMatrix = (glm::mat4*)((std::uint64_t)aModelMatrices.model + (i * dynamicAlignment));
            
            // This will need to be changed to get a 'parent' model matrix, not
            // just the first node's model matrix.
            *modelMatrix = aModels[i].nodes[0]->getModelMatrix();
        }

        std::memcpy(aBuffer.mapped, aModelMatrices.model, aModels.size() * dynamicAlignment);

        vmaFlushAllocation(aContext.allocator->allocator, aBuffer.allocation, 0, aModels.size() * dynamicAlignment);
    }
}
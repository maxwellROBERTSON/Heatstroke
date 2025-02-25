#include <cstdio>
#include <memory>
#include <chrono>

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "../Engine/vulkan/VulkanContext.hpp"
#include "../Engine/vulkan/VulkanAllocator.hpp"
#include "../Engine/vulkan/VulkanDevice.hpp"
#include "../Engine/glfw/Callbacks.hpp"
#include "../Engine/vulkan/VulkanWindow.hpp"
#include "../Engine/vulkan/Renderer.hpp"
#include "../Engine/vulkan/PipelineCreation.hpp"
#include "../Engine/vulkan/objects/Buffer.hpp"
#include "../Engine/gltf/glTF.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "Uniforms.hpp"

#include "Camera.hpp"

namespace {
    void initialiseGame(std::vector<Engine::vk::Model>& models);
    void runGameLoop(std::vector<Engine::vk::Model>& models);

    void updateSceneUniform(glsl::SceneUniform& aScene, Camera& camera, std::uint32_t aFramebufferWidth, std::uint32_t aFramebufferHeight);

    Engine::VulkanContext vkContext;

    Camera camera;
    bool recreateSwapchain = false;
}

int main() try {
    // This manual scope is very important, it ensures the objects in models have their associated
    // objects' destructors called BEFORE we destroy the vulkan device.
    {
        std::vector<Engine::vk::Model> models;
        initialiseGame(models);
        runGameLoop(models);
    }

    // vkContext would get destroyed after main exits
    // and Vulkan doesnt like objects being destroyed
    // past main, so we manually call those object 
    // destructors ourselves.
    vkContext.allocator.reset();
    vkContext.window.reset(); 

    return 0;
} catch (const std::exception& error) {
    std::fprintf(stderr, "\n");
    std::fprintf(stderr, "Error thrown: %s\n", error.what());
    return 1;
}

namespace {
    void initialiseGame(std::vector<Engine::vk::Model>& models) {

        vkContext.window = Engine::initialiseVulkan();
        vkContext.allocator = Engine::createVulkanAllocator(*vkContext.window.get());

        Engine::registerCallbacks(vkContext.getGLFWWindow());

        // Here we would load all relevant glTF models and put them in the models vector
        tinygltf::Model tinygltfmodel = Engine::loadFromFile("DamagedHelmet.gltf");
        models.emplace_back(Engine::makeVulkanModel(vkContext, tinygltfmodel));

        camera = Camera(100.0f, 0.01f, 256.0f, glm::vec3(0.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    void runGameLoop(std::vector<Engine::vk::Model>& models) {

        // Create required objects for rendering
        std::map<std::string, Engine::vk::RenderPass> renderPasses;
        renderPasses.emplace("default", Engine::createRenderPass(*vkContext.window));

        std::map<std::string, Engine::vk::DescriptorSetLayout> descriptorLayouts;
        descriptorLayouts.emplace("sceneLayout", Engine::createSceneLayout(*vkContext.window));
        descriptorLayouts.emplace("materialLayout", Engine::createMaterialLayout(*vkContext.window));
        descriptorLayouts.emplace("materialInfoLayout", Engine::createSSBOLayout(*vkContext.window));

        std::vector<VkDescriptorSetLayout> sceneDescriptorLayouts;
        sceneDescriptorLayouts.emplace_back(descriptorLayouts["sceneLayout"].handle);
        sceneDescriptorLayouts.emplace_back(descriptorLayouts["materialLayout"].handle);
        sceneDescriptorLayouts.emplace_back(descriptorLayouts["materialInfoLayout"].handle);

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
        Engine::vk::Buffer sceneUBO = Engine::vk::createBuffer(*vkContext.allocator, sizeof(glsl::SceneUniform), VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, 0, VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

        std::map<std::string, VkDescriptorSet> descriptorSets;
        descriptorSets.emplace("sceneDescriptors", Engine::createSceneDescriptor(*vkContext.window, descriptorLayouts["sceneLayout"].handle, sceneUBO.buffer));

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

            sceneUniform.model = models[0].nodes[0]->getModelMatrix();

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
                descriptorSets["sceneDescriptors"]
            );

            assert(std::size_t(frameIndex) < renderFinished.size());

            const VkResult result = Engine::submitAndPresent(*vkContext.window, cmdBuffers, frameDone, imageAvailable, renderFinished, frameIndex, imageIndex);
            if (VK_SUBOPTIMAL_KHR == result || VK_ERROR_OUT_OF_DATE_KHR == result)
			    recreateSwapchain = true;
		    else if (VK_SUCCESS != result)
			    throw Utils::Error("Unable to present swapchain image %u\n vkQueuePresentKHR() returned %s", imageIndex, Utils::toString(result).c_str());
        }

        vkDeviceWaitIdle(vkContext.window->device->device);
    }

    void updateSceneUniform(glsl::SceneUniform& aScene, Camera& camera, std::uint32_t aFramebufferWidth, std::uint32_t aFramebufferHeight) {
        const float aspectRatio = float(aFramebufferWidth / aFramebufferHeight);

        aScene.projection = glm::perspective(glm::radians(camera.fov), aspectRatio, camera.nearPlane, camera.farPlane);
        aScene.projection[1][1] *= -1.0f;
        aScene.view = glm::lookAt(camera.position, camera.position + camera.frontDirection, glm::vec3(0.0f, 1.0f, 0.0f));
        //aScene.model = glm::rotate(glm::mat4(1.0f), 90.0f, glm::vec3(1.0f, 0.0f, 0.0f));
        aScene.position = glm::vec4(camera.position, 1.0f);
    }
}
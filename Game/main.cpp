#include <cstdio>
#include <memory>

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

#include "Uniforms.hpp"

#include "Camera.hpp"


namespace {
    void initialiseGame();
    void runGameLoop();

    void updateSceneUniform(glsl::SceneUniform& aScene, Camera& camera, std::uint32_t aFramebufferWidth, std::uint32_t aFramebufferHeight);

    Engine::VulkanContext vkContext;
    Engine::vk::Model model;

    Camera camera;
}

int main() try {
    initialiseGame();
    runGameLoop();

    // vkContext would get destroyed after main exits
    // and Vulkan doesnt like objects being destroyed
    // past main, so we manually call those object 
    // destructors ourselves.
    vkContext.allocator.reset();
    vkContext.window.reset();

    return 0;
} catch (const std::exception& error ) {
    std::fprintf(stderr, "\n");
    std::fprintf(stderr, "Error thrown: %s\n", error.what());
    return 1;
}

namespace {
    void initialiseGame() {

        vkContext.window = Engine::initialiseVulkan();
        vkContext.allocator = Engine::createVulkanAllocator(*vkContext.window.get());

        Engine::registerCallbacks(vkContext.getGLFWWindow());

        tinygltf::Model tinygltfmodel = Engine::loadFromFile("DamagedHelmet.gltf");
        model = Engine::makeVulkanModel(vkContext, tinygltfmodel);

        camera = Camera(60.0f, 0.1f, 100.0f, glm::vec3(0.0f, 0.0f, 5.0f), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    void runGameLoop() {

        // Create required objects for rendering
        Engine::vk::RenderPass renderPass = Engine::createRenderPass(*vkContext.window);
        Engine::vk::DescriptorSetLayout sceneLayout = Engine::createSceneLayout(*vkContext.window);
        Engine::vk::DescriptorSetLayout materialLayout = Engine::createMaterialLayout(*vkContext.window);

        std::vector<VkDescriptorSetLayout> descriptorLayouts;
        descriptorLayouts.emplace_back(sceneLayout.handle);
        descriptorLayouts.emplace_back(materialLayout.handle);

        Engine::vk::PipelineLayout pipelineLayout = Engine::createPipelineLayout(*vkContext.window, descriptorLayouts);
        Engine::vk::Pipeline pipeline = Engine::createPipeline(*vkContext.window, renderPass.handle, pipelineLayout.handle);
        auto [depthBuffer, depthBufferView] = Engine::createDepthBuffer(*vkContext.window, *vkContext.allocator);

        // Create basic swapchain framebuffers
        std::vector<Engine::vk::Framebuffer> framebuffers;
        Engine::createFramebuffers(*vkContext.window, framebuffers, renderPass.handle, depthBufferView.handle);

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

        VkDescriptorSet sceneDescriptors = Engine::allocateDescriptorSet(*vkContext.window, vkContext.window->device->dPool, sceneLayout.handle);
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

        // Create all texture samplers
        std::vector<Engine::vk::Sampler> samplers;
        Engine::createTextureSamplers(*vkContext.window, model, samplers);

        VkDescriptorSet materialDescriptors = Engine::allocateDescriptorSet(*vkContext.window, vkContext.window->device->dPool, materialLayout.handle);
        {
            VkWriteDescriptorSet desc[5]{};

            VkDescriptorImageInfo baseColourInfo{};
            baseColourInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            baseColourInfo.imageView = model.imageViews[model.materials[0].baseColourTextureIndex].handle;
            baseColourInfo.sampler = samplers[model.textures[model.materials[0].baseColourTextureIndex].sampler].handle; // This could definitely be done better

            desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            desc[0].dstSet = materialDescriptors;
            desc[0].dstBinding = 0;
            desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc[0].descriptorCount = 1;
            desc[0].pImageInfo = &baseColourInfo;

            VkDescriptorImageInfo metallicRoughnessInfo{};
            metallicRoughnessInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            metallicRoughnessInfo.imageView = model.imageViews[model.materials[0].metallicRoughnessTextureIndex].handle;
            metallicRoughnessInfo.sampler = samplers[model.textures[model.materials[0].metallicRoughnessTextureIndex].sampler].handle;

            desc[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            desc[1].dstSet = materialDescriptors;
            desc[1].dstBinding = 1;
            desc[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc[1].descriptorCount = 1;
            desc[1].pImageInfo = &metallicRoughnessInfo;

            VkDescriptorImageInfo emissiveInfo{};
            emissiveInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            emissiveInfo.imageView = model.imageViews[model.materials[0].emissiveTextureIndex].handle;
            emissiveInfo.sampler = samplers[model.textures[model.materials[0].emissiveTextureIndex].sampler].handle;

            desc[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            desc[2].dstSet = materialDescriptors;
            desc[2].dstBinding = 2;
            desc[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc[2].descriptorCount = 1;
            desc[2].pImageInfo = &emissiveInfo;

            VkDescriptorImageInfo occlusionInfo{};
            occlusionInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            occlusionInfo.imageView = model.imageViews[model.materials[0].occlusionTextureIndex].handle;
            occlusionInfo.sampler = samplers[model.textures[model.materials[0].occlusionTextureIndex].sampler].handle;

            desc[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            desc[3].dstSet = materialDescriptors;
            desc[3].dstBinding = 3;
            desc[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc[3].descriptorCount = 1;
            desc[3].pImageInfo = &occlusionInfo;

            VkDescriptorImageInfo normalMapInfo{};
            normalMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            normalMapInfo.imageView = model.imageViews[model.materials[0].normalTextureIndex].handle;
            normalMapInfo.sampler = samplers[model.textures[model.materials[0].normalTextureIndex].sampler].handle;

            desc[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            desc[4].dstSet = materialDescriptors;
            desc[4].dstBinding = 4;
            desc[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
            desc[4].descriptorCount = 1;
            desc[4].pImageInfo = &normalMapInfo;

            constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
            vkUpdateDescriptorSets(vkContext.window->device->device, numSets, desc, 0, nullptr);
        }


        while (!glfwWindowShouldClose(vkContext.getGLFWWindow())) {
            glfwPollEvents();

            frameIndex++;
            frameIndex %= cmdBuffers.size();

            Engine::waitForFences(*vkContext.window, frameDone, frameIndex);

            std::uint32_t imageIndex = 0;
            Engine::acquireNextSwapchainImage(*vkContext.window, imageAvailable, frameIndex, imageIndex);

            Engine::resetFences(*vkContext.window, frameDone, frameIndex);

            camera.updateCamera();
            
            glsl::SceneUniform sceneUniform{};
            updateSceneUniform(sceneUniform, camera, vkContext.window->swapchainExtent.width, vkContext.window->swapchainExtent.height);

            Engine::renderModel(
                model,
                cmdBuffers[frameIndex], 
                renderPass.handle, 
                framebuffers[imageIndex].handle, 
                pipeline.handle, 
                vkContext.window->swapchainExtent,
                sceneUBO.buffer,
                sceneUniform,
                pipelineLayout.handle,
                sceneDescriptors,
                materialDescriptors
            );

            Engine::submitAndPresent(*vkContext.window, cmdBuffers, frameDone, imageAvailable, renderFinished, frameIndex, imageIndex);

        }

        vkDeviceWaitIdle(vkContext.window->device->device);
    }

    void updateSceneUniform(glsl::SceneUniform& aScene, Camera& camera, std::uint32_t aFramebufferWidth, std::uint32_t aFramebufferHeight) {
        const float aspectRatio = float(aFramebufferWidth / aFramebufferHeight);

        aScene.projection = glm::perspectiveRH_ZO(camera.fov, aspectRatio, camera.nearPlane, camera.farPlane);
        aScene.view = glm::lookAt(camera.position, camera.position + camera.frontDirection, glm::vec3(0.0f, 1.0f, 0.0f));
        aScene.position = glm::vec4(camera.position, 1.0f);
    }
}
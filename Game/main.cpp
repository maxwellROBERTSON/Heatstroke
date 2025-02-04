#include <cstdio>
#include <memory>

#include "../Engine/vulkan/VulkanAllocator.hpp"
#include "../Engine/vulkan/VulkanContext.hpp"
#include "../Engine/glfw/Callbacks.hpp"
#include "../Engine/vulkan/VulkanWindow.hpp"


namespace {
    void initialiseGame();
    void runGameLoop();

    Engine::VulkanContext vkContext;
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
    }

    void runGameLoop() {

        while (!glfwWindowShouldClose(vkContext.getGLFWWindow())) {
            glfwPollEvents();



        }
    }
}
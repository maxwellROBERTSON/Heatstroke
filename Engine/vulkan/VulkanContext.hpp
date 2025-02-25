#pragma once

#include <memory>

#include "VulkanWindow.hpp"
#include "VulkanAllocator.hpp"

namespace Engine {

    // This version of VulkanContext is slightly different to that used in the 
    // Vulkan exercises. This 'VulkanContext' is used as a type of container for
    // all things Vulkan with the engine. It will still hold the actual Vulkan context
    // (within the VulkanWindow) but is changed to also be able to initialise and 
    // contain the VulkanWindow and VulkanAllocator in one place
    struct VulkanContext {
        std::unique_ptr<VulkanWindow> window;
        std::unique_ptr<VulkanAllocator> allocator;

        GLFWwindow* getGLFWWindow();
    };

}
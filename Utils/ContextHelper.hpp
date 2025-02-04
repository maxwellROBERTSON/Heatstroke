#pragma once

// Credit: COMP5892M (Advanced Rendering)

#include <string>
#include <vector>
#include <unordered_set>

#include <volk/volk.h>

namespace Utils {
    std::unordered_set<std::string> getInstanceLayers();
    std::unordered_set<std::string> getInstanceExtensions();

    VkInstance createInstance(
        const std::vector<const char*>& aEnabledLayers = {},
        const std::vector<const char*>& aEnabledInstanceExtensions = {},
        bool aEnableDebugUtils = false
    );

    VkDebugUtilsMessengerEXT createDebugMessenger(VkInstance);

    VKAPI_ATTR VkBool32 VKAPI_CALL debugUtilCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT,
        VkDebugUtilsMessageTypeFlagsEXT,
        VkDebugUtilsMessengerCallbackDataEXT const*,
        void*
    );

    std::unordered_set<std::string> getDeviceExtensions(VkPhysicalDevice);
}
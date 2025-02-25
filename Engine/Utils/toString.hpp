#pragma once

// Credit: COMP5892M (Advanced Rendering)

#include <string>
#include <cstdint>

#include <volk/volk.h>

namespace Utils {
    std::string toString(VkResult);
    std::string toString(VkPhysicalDeviceType);
    std::string toString(VkDebugUtilsMessageSeverityFlagBitsEXT);

    std::string queueFlags(VkQueueFlags);
    std::string messageTypeFlags(VkDebugUtilsMessageTypeFlagsEXT);
    std::string memoryHeapFlags(VkMemoryHeapFlags);
    std::string memoryPropertyFlags(VkMemoryPropertyFlags);

    std::string driverVersion(std::uint32_t, std::uint32_t);
}
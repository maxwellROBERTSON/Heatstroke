#include "VulkanAllocator.hpp"

// Credit: COMP5892M (Advanced Rendering)

#include <utility>

#include "Error.hpp"
#include "toString.hpp"
#include "VulkanWindow.hpp"

namespace Engine {

	VulkanAllocator::~VulkanAllocator() {
		if (allocator != VK_NULL_HANDLE) {
			vmaDestroyAllocator(allocator);
		}
	}

	VulkanAllocator::VulkanAllocator(VmaAllocator aAllocator) noexcept
		: allocator(aAllocator) {}

	VulkanAllocator::VulkanAllocator(VulkanAllocator&& aOther) noexcept
		: allocator(std::exchange(aOther.allocator, VK_NULL_HANDLE)) {}

	VulkanAllocator& VulkanAllocator::operator=(VulkanAllocator&& aOther) noexcept {
		std::swap(allocator, aOther.allocator);
		return *this;
	}

	std::unique_ptr<VulkanAllocator> createVulkanAllocator(const VulkanWindow& aWindow) {
		VkPhysicalDeviceProperties props{};
		vkGetPhysicalDeviceProperties(aWindow.physicalDevice, &props);

		VmaVulkanFunctions functions{};
		functions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
		functions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;

		VmaAllocatorCreateInfo allocInfo{};
		allocInfo.vulkanApiVersion = props.apiVersion;
		allocInfo.physicalDevice = aWindow.physicalDevice;
		allocInfo.device = aWindow.device->device;
		allocInfo.instance = aWindow.instance;
		allocInfo.pVulkanFunctions = &functions;

		VmaAllocator allocator = VK_NULL_HANDLE;
		if (auto const res = vmaCreateAllocator(&allocInfo, &allocator); VK_SUCCESS != res)
		{
			throw Utils::Error("Unable to create allocator\n"
				"vmaCreateAllocator() returned %s", Utils::toString(res).c_str()
			);
		}

		return std::make_unique<VulkanAllocator>(allocator);
	}

}
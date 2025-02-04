#pragma once

// Adapted from: COMP5892M (Advanced Rendering)

#include <volk/volk.h>
#include <vk_mem_alloc.h>

#include "../VulkanContext.hpp"

namespace Engine {
namespace vk {

	class Buffer {
	public:
		Buffer() noexcept = default;
		~Buffer();

		explicit Buffer(VmaAllocator, VkBuffer = VK_NULL_HANDLE, VmaAllocation = VK_NULL_HANDLE) noexcept;

		Buffer(const Buffer&) = delete;
		Buffer& operator= (const Buffer&) = delete;

		Buffer(Buffer&&) noexcept;
		Buffer& operator = (Buffer&&) noexcept;

		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;

	private:
		VmaAllocator mAllocator = VK_NULL_HANDLE;
	};

	Buffer createBuffer(const VulkanContext&, VkDeviceSize, VkBufferUsageFlags, VmaAllocationCreateFlags, VmaMemoryUsage = VMA_MEMORY_USAGE_AUTO);

}
}
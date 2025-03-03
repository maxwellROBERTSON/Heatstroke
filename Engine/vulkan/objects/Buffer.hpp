#pragma once

// Adapted from: COMP5892M (Advanced Rendering)

#include <string>

#include <volk/volk.h>
#include <vk_mem_alloc.h>

#include "../VulkanAllocator.hpp"

namespace Engine {
namespace vk {

	class Buffer {
	public:
		Buffer() noexcept = default;
		~Buffer();

		explicit Buffer(VmaAllocator, std::string name, VkBuffer = VK_NULL_HANDLE, VmaAllocation = VK_NULL_HANDLE) noexcept;

		Buffer(const Buffer&) = delete;
		Buffer& operator= (const Buffer&) = delete;

		Buffer(Buffer&&) noexcept;
		Buffer& operator = (Buffer&&) noexcept;

		std::string name = ""; // Name attribute is only really here to help debug buffers that don't get destroyed appropriately and to track them down

		VkBuffer buffer = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;

		void* mapped = nullptr;

	private:
		VmaAllocator mAllocator = VK_NULL_HANDLE;
	};

	Buffer createBuffer(std::string name, const VulkanAllocator& aAllocator, VkDeviceSize aDeviceSize, VkBufferUsageFlags aUsageFlags, VmaAllocationCreateFlags aCreateFlags, VmaMemoryUsage aMemoryUsage = VMA_MEMORY_USAGE_AUTO);

}
}
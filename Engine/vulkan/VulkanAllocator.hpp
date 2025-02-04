#pragma once

// Credit: COMP5892M (Advanced Rendering)

#include <cassert>
#include <memory>

#include <volk/volk.h>
#include <vk_mem_alloc.h>


namespace Engine {
	class VulkanWindow;
	
	class VulkanAllocator {
	public:
		VulkanAllocator() noexcept = default;
		~VulkanAllocator();

		explicit VulkanAllocator(VmaAllocator) noexcept;

		VulkanAllocator(const VulkanAllocator&) = delete;
		VulkanAllocator& operator= (const VulkanAllocator&) = delete;

		VulkanAllocator(VulkanAllocator&&) noexcept;
		VulkanAllocator& operator = (VulkanAllocator&&) noexcept;

		VmaAllocator allocator = VK_NULL_HANDLE;
	};

	std::unique_ptr<VulkanAllocator> createVulkanAllocator(const VulkanWindow&);

}



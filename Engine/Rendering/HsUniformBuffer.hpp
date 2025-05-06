#pragma once

#include "../vulkan/objects/VkObjects.hpp"
#include "../vulkan/objects/Buffer.hpp"

namespace Engine {

	class VulkanAllocator;

	class HsUniformBuffer {
	public:
		HsUniformBuffer() = default;
		HsUniformBuffer(VulkanAllocator* allocator);

		virtual ~HsUniformBuffer() = default;

		virtual void update(VkCommandBuffer cmdBuf);

		VkBuffer getBufferHandle();
	protected:
		VulkanAllocator* allocator;

		vk::Buffer buffer;
	};

}
#include "HsUniformBuffer.hpp"

namespace Engine {

	HsUniformBuffer::HsUniformBuffer(VulkanAllocator* allocator) {
		this->allocator = allocator;
	}

	void HsUniformBuffer::update(VkCommandBuffer cmdBuf) {}

	VkBuffer HsUniformBuffer::getHandle() {
		return this->buffer.buffer;
	}

}
#include "HsUniformBuffer.hpp"

namespace Engine {

	HsUniformBuffer::HsUniformBuffer(VulkanAllocator* allocator) {
		this->allocator = allocator;
	}

	void HsUniformBuffer::update(VkCommandBuffer cmdBuf) {}

	VkBuffer HsUniformBuffer::getBufferHandle() {
		return this->buffer.buffer;
	}

}
#include "OrthoUniformBuffer.hpp"

#include "VulkanUtils.hpp"

OrthoUniformBuffer::OrthoUniformBuffer(Engine::VulkanAllocator* allocator, glsl::OrthoMatrices* orthoMatrices) : HsUniformBuffer(allocator) {
	this->buffer = Engine::vk::createBuffer(
		"orthoUniformBuffer",
		*this->allocator,
		sizeof(glsl::OrthoMatrices),
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		0,
		VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

	this->uniformData = orthoMatrices;
}

void OrthoUniformBuffer::update(VkCommandBuffer cmdBuf) {
	Utils::bufferBarrier(
		cmdBuf,
		this->buffer.buffer,
		VK_ACCESS_UNIFORM_READ_BIT,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT);

	vkCmdUpdateBuffer(cmdBuf, this->buffer.buffer, 0, sizeof(glsl::OrthoMatrices), this->uniformData);

	Utils::bufferBarrier(
		cmdBuf,
		this->buffer.buffer,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_UNIFORM_READ_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);
}
#include "SceneUniformBuffer.hpp"

#include "VulkanUtils.hpp"

SceneUniformBuffer::SceneUniformBuffer(Engine::VulkanAllocator* allocator, glsl::SceneUniform* sceneUniform) : HsUniformBuffer(allocator) {
	this->buffer = Engine::vk::createBuffer(
		"sceneUniformBuffer",
		*this->allocator,
		sizeof(glsl::SceneUniform),
		VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		0,
		VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

	this->uniformData = sceneUniform;
}

void SceneUniformBuffer::update(VkCommandBuffer cmdBuf) {
	Utils::bufferBarrier(
		cmdBuf,
		this->buffer.buffer,
		VK_ACCESS_UNIFORM_READ_BIT,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT);

	vkCmdUpdateBuffer(cmdBuf, this->buffer.buffer, 0, sizeof(glsl::SceneUniform), this->uniformData);

	Utils::bufferBarrier(
		cmdBuf,
		this->buffer.buffer,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_UNIFORM_READ_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_VERTEX_SHADER_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT);
}
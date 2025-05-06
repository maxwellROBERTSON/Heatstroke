#include "Crosshair.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "../vulkan/VulkanContext.hpp"
#include "../vulkan/VulkanDevice.hpp"
#include "../Utils/VulkanUtils.hpp"

Crosshair::Crosshair(Engine::VulkanContext* context) {
	this->context = context;
}

void Crosshair::drawCrosshair(VkCommandBuffer cmdBuf) {
	VkBuffer vBuffers[2] = {
		this->posBuffer.buffer,
		this->colBuffer.buffer
	};
	VkBuffer iBuffer = this->indicesBuffer.buffer;

	VkDeviceSize vOffsets[2]{};
	VkDeviceSize iOffset{};

	vkCmdBindVertexBuffers(cmdBuf, 0, 2, vBuffers, vOffsets);
	vkCmdBindIndexBuffer(cmdBuf, iBuffer, 0, VK_INDEX_TYPE_UINT32);

	vkCmdDrawIndexed(cmdBuf, this->indexCount, 1, 0, 0, 0);
}

void Crosshair::updatePositions() {
	if (!this->shouldUpdateCrosshair)
		return;

	VkExtent2D swapchainExtent = this->context->window->swapchainExtent;
	glm::vec2 midpoint = glm::vec2(swapchainExtent.width / 2.0f, swapchainExtent.height / 2.0f);

	float size = 8.0f;
	float quarterSize = size / 4.0f;
	
	this->positions.clear();
	// Vertical stalk
	this->positions.emplace_back(glm::vec2(midpoint.x - quarterSize, midpoint.y - size));
	this->positions.emplace_back(glm::vec2(midpoint.x + quarterSize, midpoint.y - size));
	this->positions.emplace_back(glm::vec2(midpoint.x - quarterSize, midpoint.y + size));
	this->positions.emplace_back(glm::vec2(midpoint.x + quarterSize, midpoint.y + size));
	// Horizontal stalk
	this->positions.emplace_back(glm::vec2(midpoint.x - size, midpoint.y + quarterSize));
	this->positions.emplace_back(glm::vec2(midpoint.x - size, midpoint.y - quarterSize));
	this->positions.emplace_back(glm::vec2(midpoint.x + size, midpoint.y - quarterSize));
	this->positions.emplace_back(glm::vec2(midpoint.x + size, midpoint.y + quarterSize));

	this->indices = { 0, 1, 2, 2, 1, 3, 4, 5, 6, 6, 7, 4 };

	this->colors.clear();
	for (std::size_t i = 0; i < 8; i++) {
		this->colors.emplace_back(this->colorValues[this->selectedColor]);
	}

	std::size_t vertexSize = this->vertexCount * sizeof(glm::vec2);
	std::size_t colorSize = this->vertexCount * sizeof(glm::vec4);
	std::size_t indexSize = this->indexCount * sizeof(std::uint32_t);

	// Since some of these GPU buffers we are about to recreative 
	// could be currently in use by one of the in-flight frames, 
	// we wait for idle before recreating them. Since this is only 
	// done on a setting change it should be fine to just use vkDeviceWaitIdle
	vkDeviceWaitIdle(this->context->window->device->device);

	// GPU buffers
	this->posBuffer = Engine::vk::createBuffer(
		"crosshairPosBuffer",
		*this->context->allocator,
		vertexSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		0,
		VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
	
	this->colBuffer = Engine::vk::createBuffer(
		"crosshairColBuffer",
		*this->context->allocator,
		colorSize,
		VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		0,
		VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

	this->indicesBuffer = Engine::vk::createBuffer(
		"crosshairIndicesBuffer",
		*this->context->allocator,
		indexSize,
		VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
		0,
		VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

	// Staging buffers
	Engine::vk::Buffer posStaging = Engine::vk::createBuffer(
		"crosshairPosStaging",
		*this->context->allocator,
		vertexSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

	Engine::vk::Buffer colStaging = Engine::vk::createBuffer(
		"crosshairColStaging",
		*this->context->allocator,
		colorSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

	Engine::vk::Buffer indicesStaging = Engine::vk::createBuffer(
		"crosshairIndicesStaging",
		*this->context->allocator,
		indexSize,
		VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
		VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

	// Copy to ptrs
	void* posPtr = nullptr;
	if (const auto res = vmaMapMemory(this->context->allocator->allocator, posStaging.allocation, &posPtr); VK_SUCCESS != res)
		throw Utils::Error("Mapping memory for memory\n vmaMapMemory() return %s\n", Utils::toString(res).c_str());

	std::memcpy(posPtr, this->positions.data(), vertexSize);
	vmaUnmapMemory(this->context->allocator->allocator, posStaging.allocation);

	void* colPtr = nullptr;
	if (const auto res = vmaMapMemory(this->context->allocator->allocator, colStaging.allocation, &colPtr); VK_SUCCESS != res)
		throw Utils::Error("Mapping memory for memory\n vmaMapMemory() return %s\n", Utils::toString(res).c_str());

	std::memcpy(colPtr, this->colors.data(), colorSize);
	vmaUnmapMemory(this->context->allocator->allocator, colStaging.allocation);

	void* indexPtr = nullptr;
	if (const auto res = vmaMapMemory(this->context->allocator->allocator, indicesStaging.allocation, &indexPtr); VK_SUCCESS != res)
		throw Utils::Error("Mapping memory for memory\n vmaMapMemory() return %s\n", Utils::toString(res).c_str());

	std::memcpy(indexPtr, this->indices.data(), indexSize);
	vmaUnmapMemory(this->context->allocator->allocator, indicesStaging.allocation);

	Engine::vk::Fence uploadComplete = Engine::createFence(*this->context->window);
	VkCommandBuffer uploadCmdBuf = Engine::createCommandBuffer(*this->context->window);

	Engine::beginCommandBuffer(uploadCmdBuf);

	VkBufferCopy posCopy{};
	posCopy.size = vertexSize;

	vkCmdCopyBuffer(uploadCmdBuf, posStaging.buffer, this->posBuffer.buffer, 1, &posCopy);

	Utils::bufferBarrier(
		uploadCmdBuf,
		this->posBuffer.buffer,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);

	VkBufferCopy colCopy{};
	colCopy.size = colorSize;

	vkCmdCopyBuffer(uploadCmdBuf, colStaging.buffer, this->colBuffer.buffer, 1, &colCopy);

	Utils::bufferBarrier(
		uploadCmdBuf,
		this->colBuffer.buffer,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);

	VkBufferCopy indicesCopy{};
	indicesCopy.size = indexSize;

	vkCmdCopyBuffer(uploadCmdBuf, indicesStaging.buffer, this->indicesBuffer.buffer, 1, &indicesCopy);

	Utils::bufferBarrier(
		uploadCmdBuf,
		this->indicesBuffer.buffer,
		VK_ACCESS_TRANSFER_WRITE_BIT,
		VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
		VK_PIPELINE_STAGE_TRANSFER_BIT,
		VK_PIPELINE_STAGE_VERTEX_INPUT_BIT);

	Engine::endAndSubmitCommandBuffer(*this->context->window, uploadCmdBuf);

	this->shouldUpdateCrosshair = false;
}
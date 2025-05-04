#include "Decals.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "VulkanUtils.hpp"
#include "../vulkan/VulkanContext.hpp"
#include "../vulkan/VulkanDevice.hpp"
#include "../vulkan/Renderer.hpp"
#include "../vulkan/PipelineCreation.hpp"

#include <glm/vec3.hpp>

namespace Engine {

	Decals::Decals(VulkanContext* context, Renderer* renderer) {
		this->context = context;
		this->renderer = renderer;

		std::size_t vertexSize = 4 * sizeof(glm::vec3);
		std::size_t indexSize = 6 * sizeof(std::uint32_t);

		std::vector<glm::vec3> vertices = {
			glm::vec3(-1.0f, -1.0f, 0.0f),
			glm::vec3(1.0f, -1.0f, 0.0f),
			glm::vec3(1.0f,  1.0f, 0.0f),
			glm::vec3(-1.0f,  1.0f, 0.0f)
		};

		std::vector<std::uint32_t> indices = { 0, 1, 2, 0, 2, 3 };
	
		this->posBuffer = Engine::vk::createBuffer(
			"decalPosBuffer",
			*this->context->allocator,
			vertexSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);
	
		this->indicesBuffer = Engine::vk::createBuffer(
			"decalIndicesBuffer",
			*this->context->allocator,
			indexSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

		// Staging buffers
		Engine::vk::Buffer posStaging = Engine::vk::createBuffer(
			"decalPosStaging",
			*this->context->allocator,
			vertexSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

		Engine::vk::Buffer indicesStaging = Engine::vk::createBuffer(
			"decalIndicesStaging",
			*this->context->allocator,
			indexSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

		void* posPtr = nullptr;
		if (const auto res = vmaMapMemory(this->context->allocator->allocator, posStaging.allocation, &posPtr); VK_SUCCESS != res)
			throw Utils::Error("Mapping memory for memory\n vmaMapMemory() return %s\n", Utils::toString(res).c_str());

		std::memcpy(posPtr, vertices.data(), vertexSize);
		vmaUnmapMemory(this->context->allocator->allocator, posStaging.allocation);

		void* indexPtr = nullptr;
		if (const auto res = vmaMapMemory(this->context->allocator->allocator, indicesStaging.allocation, &indexPtr); VK_SUCCESS != res)
			throw Utils::Error("Mapping memory for memory\n vmaMapMemory() return %s\n", Utils::toString(res).c_str());

		std::memcpy(indexPtr, indices.data(), indexSize);
		vmaUnmapMemory(this->context->allocator->allocator, indicesStaging.allocation);

		Engine::vk::Fence uploadComplete = createFence(*this->context->window);
		VkCommandBuffer uploadCmdBuf = createCommandBuffer(*this->context->window);

		beginCommandBuffer(uploadCmdBuf);

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

		endAndSubmitCommandBuffer(*this->context->window, uploadCmdBuf);

		this->decalUniform = vk::createBuffer(
			"decalTransformsUBO",
			*this->context->allocator,
			sizeof(glm::mat4) * 100,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

		this->transformDescriptorSet = Engine::createUBODescriptor(*this->context->window, this->renderer->getDescriptorLayout("vertUBOLayout"), this->decalUniform.buffer);
	}

	void Decals::setNextDecal(physx::PxVec3 position, physx::PxVec3 normal) {
		physx::PxVec3 adjustedPos = position + (normal * 0.01f);
		glm::vec3 glmNormal(normal.x, normal.y, normal.z);

		// Calculate axis and angle of rotation needed to project decal onto hit surface
		glm::vec3 axis = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), glmNormal));
		float angle = std::acos(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), glmNormal));

		if (this->leadDecal == 100)
			this->leadDecal = 0;

		this->decalTransforms[this->leadDecal] = glm::translate(glm::mat4(1.0f), glm::vec3(adjustedPos.x, adjustedPos.y, adjustedPos.z)) * glm::rotate(angle, axis) * glm::scale(glm::vec3(0.1f, 0.1f, 0.1f));
		
		if (this->activeDecals < 100)
			this->activeDecals++;

		this->leadDecal++;
	}

	void Decals::updateUniform(VkCommandBuffer cmdBuf) {
		Utils::bufferBarrier(
			cmdBuf,
			this->decalUniform.buffer,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT);

		vkCmdUpdateBuffer(cmdBuf, this->decalUniform.buffer, 0, sizeof(glm::mat4) * 100, &this->decalTransforms);

		Utils::bufferBarrier(
			cmdBuf,
			this->decalUniform.buffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_UNIFORM_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_SHADER_BIT);
	}

	void Decals::render(VkCommandBuffer cmdBuf) {
		VkBuffer vBuffers[1] = { this->posBuffer.buffer };
		VkBuffer iBuffer = this->indicesBuffer.buffer;

		VkDeviceSize vOffsets[1]{};
		VkDeviceSize iOffsets{};

		vkCmdBindVertexBuffers(cmdBuf, 0, 1, vBuffers, vOffsets);
		vkCmdBindIndexBuffer(cmdBuf, iBuffer, 0, VK_INDEX_TYPE_UINT32);

		vkCmdDrawIndexed(cmdBuf, 6, this->activeDecals, 0, 0, 0);
	}

	VkDescriptorSet Decals::getDescriptorSet() {
		return this->transformDescriptorSet;
	}

}
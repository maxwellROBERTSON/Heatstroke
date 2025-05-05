#include "Decals.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "VulkanUtils.hpp"
#include "../vulkan/VulkanContext.hpp"
#include "../vulkan/VulkanDevice.hpp"
#include "../vulkan/Renderer.hpp"
#include "../vulkan/PipelineCreation.hpp"

#include <glm/vec3.hpp>
#include <stb_image.h>

namespace Engine {

	Decals::Decals(VulkanContext* context, Renderer* renderer, std::string textureFilename) {
		this->context = context;
		this->renderer = renderer;

		this->uploadVertices();
		this->uploadImage(textureFilename);
		this->createDescriptors();
	}

	void Decals::setNextDecal(physx::PxVec3 position, physx::PxVec3 normal) {
		// Adjust position some tiny multiple of the surface normal by the number of the lead
		// decal. This does mean the 100th decal will be 0.015f off the surface the decal is
		// projected onto, but its an ok solution for combatting the z-fighting of overlapping
		// decals. A better solution would be to edit the texture of what the decal is projected
		// onto and insert the decal texture into it, but that requires editing textures at 
		// runtime and it gets more complicated if the decal spreads over more than one texture.
		physx::PxVec3 adjustedPos = position + (normal * (0.005f + (0.0001f * (this->leadDecal + 1))));
		glm::vec3 glmNormal(normal.x, normal.y, normal.z);

		// Calculate axis and angle of rotation needed to project decal onto surface
		glm::vec3 axis = glm::normalize(glm::cross(glm::vec3(0.0f, 0.0f, 1.0f), glmNormal));
		float angle = std::acos(glm::dot(glm::vec3(0.0f, 0.0f, 1.0f), glmNormal));

		// If normal of surface is same as or opposite our inital decal quad (+Z) then cross product will
		// be 0 and normalizing this will return a vec3 of nans, so we check for that and if so
		// alter axis of rotation and angle
		if (glm::isnan(axis).x || glm::isnan(axis).y || glm::isnan(axis).z) {
			axis = glmNormal;

			// If the normal of the surface is opposite our decal normal ([0, 0, 1] vs [0, 0, -1])
			// we need to rotate the decal 180 deg around the the Y-axis
			if (glmNormal.z == -1.0f) {
				axis = glm::vec3(0.0f, 1.0f, 0.0f);
				angle = glm::radians(180.0f);
			}
		}

		if (this->leadDecal == 100)
			this->leadDecal = 0;

		this->decalTransforms[this->leadDecal] = glm::translate(glm::mat4(1.0f), glm::vec3(adjustedPos.x, adjustedPos.y, adjustedPos.z)) * glm::rotate(angle, axis) * glm::scale(glm::vec3(0.025f, 0.025f, 1.0f));
		
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

	VkDescriptorSet Decals::getTransformDescriptorSet() {
		return this->transformDescriptorSet;
	}

	VkDescriptorSet Decals::getImageDescriptorSet() {
		return this->imageDescriptorSet;
	}

	int Decals::getNbActiveDecals() {
		return this->activeDecals;
	}

	void Decals::uploadVertices() {
		std::size_t vertexSize = 4 * sizeof(glm::vec3);
		std::size_t indexSize = 6 * sizeof(std::uint32_t);

		std::vector<glm::vec3> vertices = {
			glm::vec3(-1.0f,  1.0f, 0.0f), // Top Left
			glm::vec3(1.0f,  1.0f, 0.0f),  // Top Right
			glm::vec3(1.0f, -1.0f, 0.0f),  // Bottom Right
			glm::vec3(-1.0f, -1.0f, 0.0f)  // Bottom Left
		};

		std::vector<std::uint32_t> indices = { 0, 3, 1, 1, 3, 2 };

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
	}

	void Decals::uploadImage(const std::string& textureFilename) {
		// Load decal image
		int width = 0, height = 0, channels = 0;
		stbi_uc* data = stbi_load(textureFilename.c_str(), &width, &height, &channels, STBI_rgb_alpha);
		if (!data)
			throw Utils::Error("Unable to load decal texture (%s)\n", textureFilename);

		// Create VkImage and VkImageView
		TextureBufferSetting decalSetting = {
			.imageFormat = VK_FORMAT_R8G8B8A8_SRGB,
			.imageExtent = VkExtent2D { (std::uint32_t)width, (std::uint32_t)height },
			.imageUsage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
			.viewAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT
		};

		std::pair<vk::Texture, vk::ImageView> texture = createTextureBuffer(*this->context, decalSetting);
		this->image = std::move(texture.first);
		this->imageView = std::move(texture.second);

		std::uint32_t sizeInBytes = static_cast<std::uint32_t>(width * height * 4);

		vk::Buffer stagingBuf = vk::createBuffer(
			"decalStaging",
			*this->context->allocator,
			sizeInBytes,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT);

		void* dataPtr = nullptr;
		if (const auto res = vmaMapMemory(this->context->allocator->allocator, stagingBuf.allocation, &dataPtr); VK_SUCCESS != res)
			throw Utils::Error("Mapping memory for decal texture failed\n vmaMapMemory() returned %s\n", Utils::toString(res).c_str());

		std::memcpy(dataPtr, data, sizeInBytes);
		vmaUnmapMemory(this->context->allocator->allocator, stagingBuf.allocation);

		stbi_image_free(data);

		VkCommandBuffer cmdBuf = createCommandBuffer(*this->context->window);
		beginCommandBuffer(cmdBuf);

		// Transition to optimal layout
		Utils::imageBarrier(
			cmdBuf,
			this->image.image,
			0,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_IMAGE_LAYOUT_UNDEFINED,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

		Utils::copyBufferToImage(
			cmdBuf,
			stagingBuf.buffer,
			this->image.image,
			VkImageSubresourceLayers{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 0, 1 },
			VkOffset3D{ 0, 0, 0 },
			VkExtent3D{ (std::uint32_t)width, (std::uint32_t)height, 1 });

		Utils::imageBarrier(
			cmdBuf,
			this->image.image,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

		Utils::imageBarrier(
			cmdBuf,
			this->image.image,
			VK_ACCESS_TRANSFER_READ_BIT,
			VK_ACCESS_SHADER_READ_BIT,
			VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
			VkImageSubresourceRange{ VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 });

		endAndSubmitCommandBuffer(*this->context->window, cmdBuf);
	}

	void Decals::createDescriptors() {
		this->decalUniform = vk::createBuffer(
			"decalTransformsUBO",
			*this->context->allocator,
			sizeof(glm::mat4) * 100,
			VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE);

		this->transformDescriptorSet = createUBODescriptor(*this->context->window, this->renderer->getDescriptorLayout("vertUBOLayout"), this->decalUniform.buffer);

		vk::SamplerInfo samplerInfo;
		samplerInfo.magFilter = VK_FILTER_LINEAR;
		samplerInfo.minFilter = VK_FILTER_LINEAR;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		this->sampler = createTextureSampler(*this->context->window, samplerInfo);

		this->image.sampler = this->sampler.handle;

		this->imageDescriptorSet = createImageDescriptor(
			*this->context->window,
			this->renderer->getDescriptorLayout("fragImageLayout"),
			VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
			this->imageView.handle,
			this->sampler.handle);
	}

}
#pragma once

#include "../vulkan/objects/Buffer.hpp"
#include "../vulkan/objects/Texture.hpp"

#include "foundation/PxVec3.h"
#include <glm/mat4x4.hpp>

namespace Engine {

	struct VulkanContext;
	class Renderer;

	class Decals {
	public:
		Decals() = default;
		Decals(VulkanContext* context, Renderer* renderer);

		void setNextDecal(physx::PxVec3 position, physx::PxVec3 normal);

		void updateUniform(VkCommandBuffer cmdBuf);
		void render(VkCommandBuffer cmdBuf);

		VkDescriptorSet getDescriptorSet();
	private:
		VulkanContext* context;
		Renderer* renderer;

		int activeDecals = 0;
		int leadDecal = 0;

		//std::array<std::pair<bool, glm::mat4>, 100> decals;
		glm::mat4 decalTransforms[100];

		vk::Buffer posBuffer;
		vk::Buffer indicesBuffer;

		vk::Buffer decalUniform;
		VkDescriptorSet transformDescriptorSet;
	};

}

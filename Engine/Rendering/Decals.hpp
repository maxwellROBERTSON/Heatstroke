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
		Decals(VulkanContext* context, Renderer* renderer, std::string textureFilename);

		void setNextDecal(physx::PxVec3 position, physx::PxVec3 normal);

		void updateUniform(VkCommandBuffer cmdBuf);
		void render(VkCommandBuffer cmdBuf);

		VkDescriptorSet getTransformDescriptorSet();
		VkDescriptorSet getImageDescriptorSet();
		int getNbActiveDecals();
	private:
		VulkanContext* context;
		Renderer* renderer;

		int activeDecals = 0;
		int leadDecal = 0;

		glm::mat4 decalTransforms[100];

		vk::Buffer posBuffer;
		vk::Buffer indicesBuffer;

		vk::Texture image;
		vk::ImageView imageView;
		vk::Sampler sampler;

		vk::Buffer decalUniform;
		VkDescriptorSet transformDescriptorSet;
		VkDescriptorSet imageDescriptorSet;

		void uploadVertices();
		void uploadImage(const std::string& textureFilename);
		void createDescriptors();
	};

}

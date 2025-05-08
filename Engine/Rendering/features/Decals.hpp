#pragma once

#include "../../Engine/vulkan/objects/Buffer.hpp"
#include "../../Engine/vulkan/objects/Texture.hpp"

#include "foundation/PxVec3.h"
#include <glm/mat4x4.hpp>

namespace Engine {

	class Decals {
	public:
		Decals() = default;
		Decals(VulkanContext* context, std::string textureFilename, std::uint32_t maxDecals);

		void setNextDecal(physx::PxVec3 position, physx::PxVec3 normal);

		void updateUniform(VkCommandBuffer cmdBuf);
		void render(VkCommandBuffer cmdBuf);

		VkDescriptorSet getTransformDescriptorSet();
		VkDescriptorSet getImageDescriptorSet();
		int getNbActiveDecals();
	private:
		VulkanContext* context;
		std::uint32_t maxDecals = 0;

		int activeDecals = 0;
		int leadDecal = 0;

		glm::mat4 decalTransforms[100];

		vk::Buffer posBuffer;
		vk::Buffer indicesBuffer;

		vk::Texture image;
		vk::ImageView imageView;
		vk::Sampler sampler;

		vk::Buffer decalUniform;

		vk::DescriptorSetLayout transformDescriptorLayout;
		vk::DescriptorSetLayout imageDescriptorLayout;
		VkDescriptorSet transformDescriptorSet;
		VkDescriptorSet imageDescriptorSet;

		void uploadVertices();
		void uploadImage(const std::string& textureFilename);
		void createDescriptors();
	};

}


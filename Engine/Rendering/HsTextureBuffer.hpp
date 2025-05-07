#pragma once

#include "../vulkan/objects/VkObjects.hpp"
#include "../vulkan/objects/Texture.hpp"

namespace Engine {

	struct VulkanContext;

	class HsTextureBuffer {
	public:
		HsTextureBuffer() = default;
		HsTextureBuffer(VulkanContext* context);

		virtual ~HsTextureBuffer() = default;

		virtual void recreate();

		vk::Texture& getImage();
		vk::ImageView& getImageView();

	protected:
		VulkanContext* context;

		vk::Texture image;
		vk::ImageView imageView;

		VkSampleCountFlagBits* sampleCount = nullptr;
	};

}
#pragma once

#include <volk/volk.h>
#include <vk_mem_alloc.h>
#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "VkObjects.hpp"

namespace Engine {

	struct VulkanContext;
	class VulkanWindow;

namespace vk {

	class Texture {
	public:
		Texture() noexcept = default;
		~Texture();

		explicit Texture(VmaAllocator, std::string name, VkImage = VK_NULL_HANDLE, VmaAllocation = VK_NULL_HANDLE) noexcept;

		Texture(const Texture&) = delete;
		Texture& operator= (const Texture&) = delete;

		Texture(Texture&&) noexcept;
		Texture& operator= (Texture&&) noexcept;

		std::string name = "";

		VkImage image = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;

		VkSampler sampler = VK_NULL_HANDLE;

	private:
		VmaAllocator mAllocator = VK_NULL_HANDLE;
	};

	Texture createTexture(const VulkanContext& aContext, std::string name, tinygltf::Image aTinygltfImage, VkFormat aFormat, VkSampler aSampler);
	Texture createDummyTexture(const VulkanContext& aContext, VkSampler aSampler);

	ImageView createImageView(const VulkanWindow& aWindow, VkImage aImage, VkFormat aFormat);

	std::uint32_t computeMipLevels(std::uint32_t width, std::uint32_t height);

	Texture createVkImage(const VulkanContext&, std::string, std::uint32_t, std::uint32_t, VkFormat, std::uint32_t, VkImageUsageFlags);

}
}
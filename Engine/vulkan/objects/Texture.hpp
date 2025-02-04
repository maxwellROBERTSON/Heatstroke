#pragma once

#include <volk/volk.h>
#include <vk_mem_alloc.h>
#include <tiny_gltf.h>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

#include "../VulkanContext.hpp"

namespace Engine {
namespace vk {

	struct SamplerInfo {
		VkFilter minFilter;
		VkFilter magFilter;
		VkSamplerAddressMode addressModeU;
		VkSamplerAddressMode addressModeV;
	};

	enum AlphaMode {
		ALPHA_OPAQUE,
		ALPHA_MASK,
		ALPHA_BLEND
	};

	struct Material {
		AlphaMode alphaMode;
		float alphaCutoff;

		glm::vec3 emissiveFactor;
		int emissiveTextureIndex;
		int emissiveTextureTexCoords;

		int normalTextureIndex;
		int normalTextureTexCoords;

		int occlusionTextureIndex;
		int occlusionTextureTexCoords;
		float occlusionStrength;

		glm::vec4 baseColourFactor;
		int baseColourTextureIndex;
		int baseColourTextureTexCoords;

		int metallicRoughnessTextureIndex;
		int metallicRoughnessTextureTexCoords;
		float metallicFactor;
		float roughnessFactor;

	};

	class Texture {
	public:
		Texture() noexcept = default;
		~Texture();

		explicit Texture(VmaAllocator, VkImage = VK_NULL_HANDLE, VmaAllocation = VK_NULL_HANDLE) noexcept;

		Texture(const Texture&) = delete;
		Texture& operator= (const Texture&) = delete;

		Texture(Texture&&) noexcept;
		Texture& operator= (Texture&&) noexcept;

		VkImage image = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;

	private:
		VmaAllocator mAllocator = VK_NULL_HANDLE;
	};

	Texture createTexture(const VulkanContext&, tinygltf::Image, VkFormat);
	ImageView createImageView(const VulkanWindow&, VkImage, VkFormat);

	std::uint32_t computeMipLevels(std::uint32_t width, std::uint32_t height);

	Texture createVkImage(const VulkanContext&, std::uint32_t, std::uint32_t, VkFormat, std::uint32_t, VkImageUsageFlags);

}
}
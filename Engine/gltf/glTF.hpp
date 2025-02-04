#pragma once

#include <string>

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include "../vulkan/objects/Texture.hpp"
#include "../vulkan/VulkanContext.hpp"

namespace Engine {

	struct Model {
		std::vector<vk::Material> materials;
		std::vector<vk::SamplerInfo> samplers;
		std::vector<vk::Texture> textures;
		std::vector<vk::ImageView> imageViews;
	};

	tinygltf::Model loadFromFile(const std::string& filename);

	Model makeVulkanModel(const VulkanContext& aContext, tinygltf::Model model);

	void loadMaterials(tinygltf::Model model, Model& vkModel);

	void loadTextureSamplers(tinygltf::Model model, Model& vkModel);

	void loadTextures(const VulkanContext& aContext, tinygltf::Model model, Model& vkModel);

}
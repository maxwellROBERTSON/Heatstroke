#pragma once

#include <string>

#define TINYGLTF_NO_STB_IMAGE_WRITE
#include <tiny_gltf.h>

#include "../vulkan/objects/Model.hpp"
#include "../vulkan/VulkanContext.hpp"

namespace Engine {
	tinygltf::Model loadFromFile(const std::string& filename);

	vk::Model makeVulkanModel(const VulkanContext& aContext, tinygltf::Model& model);

	void loadMaterials(tinygltf::Model& model, vk::Model& vkModel);

	void loadTextureSamplers(tinygltf::Model& model, vk::Model& vkModel);

	void loadTextures(const VulkanContext& aContext, tinygltf::Model& model, vk::Model& vkModel);

	void getCounts(tinygltf::Model& model, tinygltf::Node& node, std::uint32_t& indicesCount, std::uint32_t& verticesCount);

	// void loadNodeMeshes(vk::Node* parent, tinygltf::Node& node, std::uint32_t nodeIndex);

}
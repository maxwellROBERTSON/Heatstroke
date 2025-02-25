#pragma once

#include <string>

#include "../vulkan/objects/Model.hpp"
#include "../vulkan/VulkanContext.hpp"

namespace Engine {
	tinygltf::Model loadFromFile(const std::string& filename);

	vk::Model makeVulkanModel(const VulkanContext& aContext, tinygltf::Model& model);

	void loadMaterials(tinygltf::Model& model, vk::Model& vkModel);

	void loadTextureSamplers(const VulkanWindow& aWindow, tinygltf::Model& model, vk::Model& vkModel);

	void loadTextures(const VulkanContext& aContext, tinygltf::Model& model, vk::Model& vkModel);

	void getCounts(tinygltf::Model& model, tinygltf::Node& node, std::uint32_t& indicesCount, std::uint32_t& verticesCount);

	void loadNodeMeshes(vk::Node* parent, tinygltf::Node& node, tinygltf::Model& model, std::uint32_t nodeIndex, vk::RawData& rawData, vk::Model& vkModel);

	void createVulkanBuffers(const VulkanContext& aContext, vk::Model& vkModel, vk::RawData& rawData);
}
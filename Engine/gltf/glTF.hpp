#pragma once

#include <string>

#include "Model.hpp"
#include "../vulkan/VulkanContext.hpp"

namespace Engine {
	tinygltf::Model loadFromFile(const std::string& filename);

	vk::Model makeVulkanModel(const VulkanContext& aContext, tinygltf::Model& model);

	void loadMaterials(tinygltf::Model& model, vk::Model& vkModel);
	void loadTextureSamplers(const VulkanWindow& aWindow, tinygltf::Model& model, vk::Model& vkModel);
	void loadTextures(const VulkanContext& aContext, tinygltf::Model& model, vk::Model& vkModel);
	void loadNodes(vk::Node* parent, tinygltf::Node& node, tinygltf::Model& model, std::uint32_t nodeIndex, vk::Model& vkModel);
	void calculateModelBoundingBox(vk::Model& vkModel);
	void loadAnimations(tinygltf::Model& model, vk::Model& vkModel);
	void loadSkins(tinygltf::Model& model, vk::Model& vkModel);

	void createVulkanBuffers(const VulkanContext& aContext, vk::Model& vkModel);
}
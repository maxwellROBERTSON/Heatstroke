#pragma once

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Texture.hpp"

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

	struct Primitive {
		std::uint32_t firstIndex;
		std::uint32_t indexCount;
		std::uint32_t vertexCount;
		Material* material;
	};

	struct Mesh {
		std::vector<Primitive*> primitives;
	};

	struct Node {
		Node* parent;
		Mesh* mesh;
		std::uint32_t index;
		std::vector<Node*> children;
		glm::mat4 nodeMatrix;
		glm::vec3 translation;
		glm::vec3 scale{1.f};
		glm::quat rotation;
	};

    struct Model {
        std::vector<vk::Material> materials;
        std::vector<vk::SamplerInfo> samplers;
        std::vector<vk::Texture> textures;
        std::vector<vk::ImageView> imageViews;

        void drawModel();
    };

}
}
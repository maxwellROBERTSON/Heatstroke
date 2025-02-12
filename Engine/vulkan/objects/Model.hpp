#pragma once

#include <vector>

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Texture.hpp"
#include "Buffer.hpp"

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
		Primitive(std::uint32_t firstIndex, std::uint32_t indexCount, std::uint32_t vertexCount, Material& material) :
			firstIndex(firstIndex), indexCount(indexCount), vertexCount(vertexCount), material(material) {
				hasIndices = indexCount > 0;
			};

		std::uint32_t firstIndex;
		std::uint32_t indexCount;
		std::uint32_t vertexCount;
		Material& material;
		bool hasIndices;
	};

	struct Mesh {
		Mesh(glm::mat4 matrix) : matrix(matrix) {};
		std::vector<Primitive*> primitives;
		glm::mat4 matrix;
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
		std::vector<Node*> nodes;
        std::vector<Material> materials;
        std::vector<SamplerInfo> samplerInfos;
        std::vector<Texture> textures;
        std::vector<ImageView> imageViews;

		Buffer posBuffer;
		Buffer normBuffer;
		Buffer texBuffer;
		Buffer vertColBuffer;
		Buffer indicesBuffer;

        void drawModel(VkCommandBuffer aCmdBuf);
		void drawNode(Node* node, VkCommandBuffer aCmdBuf);
		void destroy();
	};

	struct RawData {
		RawData(std::uint32_t vertexCount, std::uint32_t indexCount) {
			positions.reserve(vertexCount);
			normals.reserve(vertexCount);
			texCoords.reserve(vertexCount);
			vertexColours.reserve(vertexCount);
			indices.reserve(indexCount);
		};

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec2> texCoords;
		std::vector<glm::vec4> vertexColours;
		std::vector<std::uint32_t> indices;
	};

}
}
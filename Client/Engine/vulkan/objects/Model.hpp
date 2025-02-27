#pragma once

#include <vector>
#include <iostream>

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
		ALPHA_OPAQUE = 0,
		ALPHA_MASK = 1,
		ALPHA_BLEND = 2
	};

	struct Material {
		int index;

		AlphaMode alphaMode;
		float alphaCutoff;

		glm::vec3 emissiveFactor;
		int emissiveTextureIndex;
		int emissiveTextureTexCoords;
		float emissiveStrength;

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

	struct RawData {
		RawData(std::uint32_t vertexCount, std::uint32_t indexCount) {
			positions.reserve(vertexCount);
			normals.reserve(vertexCount);
			texCoords0.reserve(vertexCount);
			texCoords1.reserve(vertexCount);
			vertexColours.reserve(vertexCount);
			indices.reserve(indexCount);
		};

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec4> tangents;
		std::vector<glm::vec2> texCoords0;
		std::vector<glm::vec2> texCoords1;
		std::vector<glm::vec4> vertexColours;
		std::vector<std::uint32_t> indices;
	};

	struct Primitive {
		Primitive(std::uint32_t firstIndex, std::uint32_t indexCount, std::uint32_t vertexCount, RawData rawData, Material& material) :
			firstIndex(firstIndex), indexCount(indexCount), vertexCount(vertexCount), rawData(rawData), material(material) {
				hasIndices = indexCount > 0;
			};

		std::uint32_t firstIndex;
		std::uint32_t indexCount;
		std::uint32_t vertexCount;
		bool hasIndices;

		Material& material;

		RawData rawData;
		
		Buffer posBuffer;
		Buffer normBuffer;
		Buffer tangentBuffer;
		Buffer tex0Buffer;
		Buffer tex1Buffer;
		Buffer vertColBuffer;
		Buffer indicesBuffer;

		VkDescriptorSet samplerDescriptorSet;
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

		glm::mat4 postTransform{ 1.0f };

		glm::mat4 getModelMatrix();
		// This method sets the transform that gets applied AFTER
		// the transforms from the glTF file have been applied.
		// This method will be moved out of the node part of the model
		// eventually
		void setPostTransform(glm::mat4 transform) {
			this->postTransform = transform;
		}
	};

    struct Model {
		std::vector<Node*> nodes;
        std::vector<Material> materials;
        std::vector<SamplerInfo> samplerInfos; // Sampler info from tinygltf
		std::vector<Sampler> samplers; // Actual Vulkan sampler objects
		Sampler defaultSampler; // Default sampler for when a texture doesn't reference any sampler
        std::vector<Texture> textures;
        std::vector<ImageView> imageViews;
		
		ImageView dummyImageView;
		Texture dummyTexture;

		Buffer materialInfoBuffer;

		VkIndexType indexType;

		VkDescriptorSet materialInfoSSBO;

		void createDescriptorSets(
			const VulkanContext& aContext, 
			VkDescriptorSetLayout aSamplerSetLayout, 
			VkDescriptorSetLayout aMaterialInfoSetLayout);
        void drawModel(VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout);
		void drawNode(Node* node, VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout);
		void destroy();
	};

}
}
#pragma once

#include <vector>
#include <iostream>

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Texture.hpp"
#include "Buffer.hpp"
#include "../gltf/Animation.hpp"
#include "../gltf/Skin.hpp"

namespace Engine {

	class Renderer;

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
			joints.reserve(vertexCount);
			weights.reserve(vertexCount);
			indices.reserve(indexCount);
		};

		std::vector<glm::vec3> positions;
		std::vector<glm::vec3> normals;
		std::vector<glm::vec4> tangents;
		std::vector<glm::vec2> texCoords0;
		std::vector<glm::vec2> texCoords1;
		std::vector<glm::vec4> vertexColours;
		std::vector<glm::uvec4> joints;
		std::vector<glm::vec4> weights;
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
		Buffer jointsBuffer;
		Buffer weightsBuffer;
		Buffer indicesBuffer;

		VkDescriptorSet samplerDescriptorSet;

		// Bounding box should be able to be created with these values.
		// They should represent the two opposite corners of the box that
		// contains every vertex of this primitive
		glm::vec3 min;
		glm::vec3 max;

		void setBounds(glm::vec3 min, glm::vec3 max) {
			this->min = min;
			this->max = max;
		}
	};

	struct Mesh {
		Mesh(glm::mat4 matrix) : matrix(matrix) {};
		std::vector<Primitive*> primitives;
		glm::mat4 matrix;
	};

	struct Node {
		Node* parent;
		Mesh* mesh;
		Skin* skin;
		std::uint32_t skinIndex = -1;
		std::uint32_t index;
		std::vector<Node*> children;

		glm::mat4 nodeMatrix;
		glm::vec3 translation;
		glm::quat rotation;
		glm::vec3 scale{ 1.0f };

		// This gets the matrix transformation local to this node.
		// You most likely will want the getModelMatrix() method
		// which returns the global transformation matrix for this node.
		glm::mat4 getLocalMatrix();
		// Returns global transformation matrix for this node.
		glm::mat4 getModelMatrix();

		// Bounding box minimums and maximums of this node
		glm::vec3 bbMin;
		glm::vec3 bbMax;

		glsl::SkinningUniform skinUniform;
	};

    struct Model {
		std::vector<Node*> nodes;
        std::vector<Material> materials;
        std::vector<SamplerInfo> samplerInfos; // Sampler info from tinygltf
		std::vector<Sampler> samplers; // Actual Vulkan sampler objects
        std::vector<Texture> textures;
        std::vector<ImageView> imageViews;
		std::vector<Animation> animations;
		std::vector<Skin> skins;

		Sampler defaultSampler; // Default sampler for when a texture doesn't reference any sampler
		ImageView dummyImageView;
		Texture dummyTexture;

		Buffer materialInfoBuffer;

		VkIndexType indexType;

		VkDescriptorSet materialInfoSSBO;

		int meshedNodes;

		// Bounding box of the entire model
		glm::vec3 bbMin;
		glm::vec3 bbMax;

		void createDescriptorSets(
			const VulkanContext& aContext, 
			VkDescriptorSetLayout aSamplerSetLayout, 
			VkDescriptorSetLayout aMaterialInfoSetLayout);
        void drawModel(VkCommandBuffer aCmdBuf, Renderer* aRenderer, const std::string& aPipelineHandle, int modelMatricesSet, std::uint32_t& offset, bool justGeometry = false);
		void drawNode(Node* node, VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout, AlphaMode aAlphaMode);
		void drawNodeGeometry(Node* node, VkCommandBuffer aCmdBuf, AlphaMode aAlphaMode);

		Node* getNodeFromIndex(int nodeIndex);

		void destroy();
	};

}
}
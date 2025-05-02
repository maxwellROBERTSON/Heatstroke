#pragma once

#include <vector>
#include <iostream>
#include <queue>

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Uniforms.hpp"
#include "Animation.hpp"
#include "Skin.hpp"
#include "Node.hpp"
#include "../vulkan/objects/Texture.hpp"
#include "../vulkan/objects/Buffer.hpp"

namespace Engine {

	class Renderer;

	enum class DrawType {
		WORLD,
		OVERLAY
	};

namespace vk {

    struct SamplerInfo {
		VkFilter minFilter;
		VkFilter magFilter;
		VkSamplerAddressMode addressModeU;
		VkSamplerAddressMode addressModeV;
		VkBool32 compareEnable = 0;
		VkCompareOp compareOp = VK_COMPARE_OP_NEVER;
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

    struct Model {
		// Vector of root nodes for each scene (usually just 1)
		std::vector<Node*> nodes;
		// Vector of all nodes linearly (allows for easily iterating over all nodes)
		std::vector<Node*> linearNodes;
        std::vector<Material> materials;
        std::vector<SamplerInfo> samplerInfos; // Sampler info from tinygltf
		std::vector<Sampler> samplers; // Actual Vulkan sampler objects
        std::vector<Texture> textures;
        std::vector<ImageView> imageViews;
		std::vector<Animation> animations;
		std::vector<Skin*> skins;

		DrawType drawType;

		Sampler defaultSampler; // Default sampler for when a texture doesn't reference any sampler
		ImageView dummyImageView;
		Texture dummyTexture;

		Buffer materialInfoBuffer;
		VkIndexType indexType;
		VkDescriptorSet materialInfoSSBO;

		int meshedNodes;

		int animationIndex = 0; // TEMP
		// We always want an idle animation for any model that has animations (or at least try to)
		Animation idleAnimation;
		std::queue<Animation> animationQueue;
		bool blending = false;
		float blendingTimer = 0.0f;

		// Bounding box of the entire model
		glm::vec3 bbMin;
		glm::vec3 bbMax;

		void createDescriptorSets(const VulkanContext& aContext, VkDescriptorSetLayout aSamplerSetLayout, VkDescriptorSetLayout aMaterialInfoSetLayout);
        void drawModel(VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout, bool justGeometry = false);
		void drawNode(Node* node, VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout, AlphaMode aAlphaMode);
		void drawNodeGeometry(Node* node, VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout, AlphaMode aAlphaMode);

		void playAnimation();
		void updateAnimation(float timeDelta);
		void blendAnimation(Animation& target, float timeDelta, float interpolationTime);

		Node* getNodeFromIndex(int nodeIndex);

		void destroy();
	};

}
}
#include "glTF.hpp"

#define TINYGLTF_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <glm/gtc/type_ptr.inl>

#include "Error.hpp"
#include "VulkanEnums.hpp"
#include "../vulkan/objects/Texture.hpp"

namespace Engine {

	tinygltf::Model loadFromFile(const std::string& filename) {
		tinygltf::Model model;
		tinygltf::TinyGLTF loader;

		std::string error;
		std::string warning;

		bool binaryFile = false;
		std::size_t extensionPos = filename.rfind(".", filename.length());
		if (extensionPos != std::string::npos)
			binaryFile = filename.substr(extensionPos + 1, filename.length() - extensionPos) == "glb";

		bool result = binaryFile ? loader.LoadBinaryFromFile(&model, &error, &warning, filename) : loader.LoadASCIIFromFile(&model, &error, &warning, filename);

		if (!error.empty())
			std::fprintf(stderr, "Failed to load glTF file %s. Error: %s\n", filename.c_str(), error.c_str());

		if (!warning.empty())
			std::fprintf(stderr, "Warning during loading glTF file %s. Warning: %s\n", filename.c_str(), warning.c_str());

		if (!result)
			throw Utils::Error("Failed to parse glTF file %s.", filename.c_str());

		return model;
	}

	vk::Model makeVulkanModel(const VulkanContext& aContext, tinygltf::Model& model) {
		vk::Model vkModel{};

		// 1. Load material info
		loadMaterials(model, vkModel);

		// 2. Load texture samplers
		loadTextureSamplers(model, vkModel);

		// 3. Load textures
		loadTextures(aContext, model, vkModel);

		// 4. Determine default scene
		const tinygltf::Scene& scene = model.scenes[model.defaultScene > -1 ? model.defaultScene : 0];

		// 5. Recurse through scene nodes and get mesh data for each

		std::uint32_t indicesCount, verticesCount;

		// Get indices and vertices count for all nodes in the scene
		for (std::size_t i = 0; i < model.nodes.size(); i++)
			getCounts(model, model.nodes[scene.nodes[i]], indicesCount, verticesCount);

		// Load the node meshes into Vulkan objects
		for (std::size_t i = 0; scene.nodes.size(); i++) {
			const tinygltf::Node node = model.nodes[scene.nodes[i]];
			// loadNodeMeshes()
		}

		return vkModel;
	}

	void loadMaterials(tinygltf::Model& model, vk::Model& vkModel) {
		for (tinygltf::Material material : model.materials) {
			vk::Material vkMaterial{};
			
			if (material.alphaMode == "MASK") {
				vkMaterial.alphaMode = vk::AlphaMode::ALPHA_MASK;
				vkMaterial.alphaCutoff = material.alphaCutoff;
			}
			else if (material.alphaMode == "BLEND") {
				vkMaterial.alphaMode = vk::AlphaMode::ALPHA_BLEND;
			}
			else {
				vkMaterial.alphaMode = vk::AlphaMode::ALPHA_OPAQUE;
			}

			vkMaterial.emissiveFactor = glm::make_vec3(material.emissiveFactor.data());
			vkMaterial.emissiveTextureIndex = material.emissiveTexture.index;
			vkMaterial.emissiveTextureTexCoords = material.emissiveTexture.texCoord;

			vkMaterial.normalTextureIndex = material.normalTexture.index;
			vkMaterial.normalTextureTexCoords = material.normalTexture.texCoord;

			vkMaterial.occlusionTextureIndex = material.occlusionTexture.index;
			vkMaterial.occlusionTextureTexCoords = material.occlusionTexture.texCoord;
			vkMaterial.occlusionStrength = material.occlusionTexture.strength;

			vkMaterial.baseColourFactor = glm::make_vec4(material.pbrMetallicRoughness.baseColorFactor.data());
			vkMaterial.baseColourTextureIndex = material.pbrMetallicRoughness.baseColorTexture.index;
			vkMaterial.baseColourTextureTexCoords = material.pbrMetallicRoughness.baseColorTexture.texCoord;

			vkMaterial.metallicRoughnessTextureIndex = material.pbrMetallicRoughness.metallicRoughnessTexture.index;
			vkMaterial.metallicRoughnessTextureTexCoords = material.pbrMetallicRoughness.metallicRoughnessTexture.texCoord;
			vkMaterial.metallicFactor = material.pbrMetallicRoughness.metallicFactor;
			vkMaterial.roughnessFactor = material.pbrMetallicRoughness.roughnessFactor;
			
			vkModel.materials.emplace_back(vkMaterial);
		}
	}

	void loadTextureSamplers(tinygltf::Model& model, vk::Model& vkModel) {
		for (tinygltf::Sampler sampler : model.samplers) {
			vk::SamplerInfo vkSampler{};
			vkSampler.minFilter = Utils::getVkFilter(sampler.minFilter);
			vkSampler.magFilter = Utils::getVkFilter(sampler.magFilter);
			vkSampler.addressModeU = Utils::getVkSamplerAddressMode(sampler.wrapS);
			vkSampler.addressModeV = Utils::getVkSamplerAddressMode(sampler.wrapT);
			
			vkModel.samplers.emplace_back(vkSampler);
		}
	}

	void loadTextures(const VulkanContext& aContext, tinygltf::Model& model, vk::Model& vkModel) {
		for (std::size_t i = 0; i < model.textures.size(); i++) {
			tinygltf::Texture texture = model.textures[i];

			vk::SamplerInfo sampler;
			// Check if texture references an existing sampler or not
			if (texture.sampler == -1) {
				// Texture does not reference an existing sampler so we assign it a default one
				sampler.minFilter = VK_FILTER_NEAREST;
				sampler.magFilter = VK_FILTER_NEAREST;
				sampler.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
				sampler.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
			}
			else {
				// Get the referenced sampler
				sampler = vkModel.samplers[texture.sampler];
			}

			// Get image the texture references
			int textureSource = texture.source;
			tinygltf::Image image = model.images[textureSource];

			VkFormat format = VK_FORMAT_R8G8B8A8_UNORM;

			// Go through all materials and if the baseColourTextureIndex is this texture
			// then set format to SRGB format (potentially an expensive operation)
			for (const vk::Material& material : vkModel.materials) {
				if (material.baseColourTextureIndex == i)
					format = VK_FORMAT_R8G8B8A8_SRGB;
			}

			vk::Texture vkTexture = vk::createTexture(aContext, image, format);
			vk::ImageView vkImageView = vk::createImageView(*aContext.window, vkTexture.image, format);

			vkModel.textures.emplace_back(std::move(vkTexture));
			vkModel.imageViews.emplace_back(std::move(vkImageView));
		}
	}

	void getCounts(tinygltf::Model& model, tinygltf::Node& node, std::uint32_t& indicesCount, std::uint32_t& verticesCount) {
		if (!node.children.empty()) {
			for (std::size_t i = 0; i < node.children.size(); i++)
				getCounts(model, model.nodes[node.children[i]], indicesCount, verticesCount);
		}

		if (node.mesh > -1) {
			const tinygltf::Mesh mesh = model.meshes[node.mesh];

			for (std::size_t i = 0; i < mesh.primitives.size(); i++) {
				tinygltf::Primitive primitive = mesh.primitives[i];
				verticesCount += model.accessors[primitive.attributes.find("POSITION")->second].count;

				if (primitive.indices > -1)
					indicesCount += model.accessors[primitive.indices].count;
			}
		}
	}


}
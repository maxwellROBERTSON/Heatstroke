#include "glTF.hpp"

#include <glm/gtc/type_ptr.inl>

#include "Error.hpp"
#include "VulkanEnums.hpp"

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
		std::fprintf(stdout, "In makeVulkanModel\n");

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

		vk::RawData rawData(verticesCount, indicesCount);

		std::fprintf(stdout, "Before loadNodeMeshes\nscene.nodes.size(): %zu\n", scene.nodes.size());

		// Load the node meshes into Vulkan objects
		for (std::size_t i = 0; scene.nodes.size(); i++) {
			tinygltf::Node node = model.nodes[scene.nodes[i]];
			loadNodeMeshes(nullptr, node, model, scene.nodes[i], rawData, vkModel);
		}

		std::fprintf(stdout, "Before createVulkanBuffers\n");

		createVulkanBuffers(vkModel, rawData);

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
			
			vkModel.samplerInfos.emplace_back(vkSampler);
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
				sampler = vkModel.samplerInfos[texture.sampler];
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

	void loadNodeMeshes(vk::Node* parent, tinygltf::Node& node, tinygltf::Model& model, std::uint32_t nodeIndex, vk::RawData& rawData, vk::Model& vkModel) {
		vk::Node* newNode = new vk::Node();
		std::fprintf(stdout, "After newNode\n");
		newNode->index = nodeIndex;
		newNode->parent = parent;
		newNode->nodeMatrix = glm::mat4(1.0f);

		glm::vec3 translation(0.0f);
		if (node.translation.size() == 3) {
			translation = glm::make_vec3(node.translation.data());
			newNode->translation = translation;
		}

		if (node.rotation.size() == 4) {
			glm::quat quaternion = glm::make_quat(node.rotation.data());
			newNode->rotation = glm::mat4(quaternion);
		}

		glm::vec3 scale(1.0f);
		if (node.scale.size() == 3) {
			scale = glm::make_vec3(node.scale.data());
			newNode->scale = scale;
		}

		if (node.matrix.size() == 16)
			newNode->nodeMatrix = glm::make_mat4x4(node.matrix.data());

		std::fprintf(stdout, "node.children.size(): %zu\nnode.children.empty(): %i\n", node.children.size(), node.children.empty());

		if (!node.children.empty()) {
			std::fprintf(stdout, "In !node.children.empty()\n");
			for (std::size_t i = 0; i < node.children.size(); i++)
				loadNodeMeshes(newNode, model.nodes[node.children[i]], model, node.children[i], rawData, vkModel);
		}

		if (node.mesh > -1) {
			const tinygltf::Mesh mesh = model.meshes[node.mesh];
			vk::Mesh* newMesh = new vk::Mesh(newNode->nodeMatrix);

			for (std::size_t i = 0; i < mesh.primitives.size(); i++) {
				const tinygltf::Primitive& primitive = mesh.primitives[i];

				// std::uint32_t indexStart, vertexStart;
				std::uint32_t indexCount = 0, vertexCount = 0;
				bool hasIndices = primitive.indices > -1;

				const float* bufferPos = nullptr;
				const float* normalsPos = nullptr;
				const float* texCoordsPos = nullptr;
				const float* colourPos = nullptr;
				int positionByteStride;
				int normalsByteStride;
				int texCoordsByteStride;
				int colourByteStride;

				const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& positionBufView = model.bufferViews[positionAccessor.bufferView];
				bufferPos = reinterpret_cast<const float*>(&(model.buffers[positionBufView.buffer].data[positionAccessor.byteOffset + positionBufView.byteOffset]));
				vertexCount = positionAccessor.count;
				positionByteStride = positionAccessor.ByteStride(positionBufView) ? (positionAccessor.ByteStride(positionBufView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

				// Get normals
				if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
					const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& normalBufView = model.bufferViews[normalAccessor.bufferView];
					normalsPos = reinterpret_cast<const float*>(&(model.buffers[normalBufView.buffer].data[normalAccessor.byteOffset + normalBufView.byteOffset]));
					normalsByteStride = normalAccessor.ByteStride(normalBufView) ? (normalAccessor.ByteStride(normalBufView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
				}
				
				// Get texCoords
				if (primitive.attributes.find("TEXCOORD_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& texCoordAccessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& texCoordBufView = model.bufferViews[texCoordAccessor.bufferView];
					texCoordsPos = reinterpret_cast<const float*>(&(model.buffers[texCoordBufView.buffer].data[texCoordAccessor.byteOffset + texCoordBufView.byteOffset]));
					texCoordsByteStride = texCoordAccessor.ByteStride(texCoordBufView) ? (texCoordAccessor.ByteStride(texCoordBufView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
				}
				
				// Get colour
				if (primitive.attributes.find("COLOR_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& colourAccessor = model.accessors[primitive.attributes.find("COLOR_0")->second];
					const tinygltf::BufferView& colourBufView = model.bufferViews[colourAccessor.bufferView];
					colourPos = reinterpret_cast<const float*>(&(model.buffers[colourBufView.buffer].data[colourAccessor.byteOffset + colourBufView.byteOffset]));
					colourByteStride = colourAccessor.ByteStride(colourBufView) ? (colourAccessor.ByteStride(colourBufView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
				}

				for (std::size_t j = 0; j < positionAccessor.count; j++) {
					rawData.positions.emplace_back(glm::make_vec3(&bufferPos[j * positionByteStride]));
					rawData.normals.emplace_back(glm::normalize(glm::vec3(normalsPos ? glm::make_vec3(&normalsPos[j * normalsByteStride]) : glm::vec3(0.0f))));
					rawData.texCoords.emplace_back(texCoordsPos ? glm::make_vec2(&texCoordsPos[j * texCoordsByteStride]) : glm::vec2(0.0f));
					rawData.vertexColours.emplace_back(colourPos ? glm::make_vec4(&colourPos[j * colourByteStride]) : glm::vec4(1.0f));
				}

				if (hasIndices) {
					const tinygltf::Accessor& indicesAccessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
					const tinygltf::BufferView& indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
					const tinygltf::Buffer& indicesBuffer = model.buffers[indicesBufferView.buffer];

					indexCount = indicesAccessor.count;
					const void* dataPtr = &(indicesBuffer.data[indicesAccessor.byteOffset + indicesBufferView.byteOffset]);
				
					switch (indicesAccessor.componentType) {
						case TINYGLTF_PARAMETER_TYPE_UNSIGNED_INT: {
							const std::uint32_t* buffer = (const std::uint32_t*)dataPtr;
							for (std::size_t k = 0; k < indicesAccessor.componentType; k++)
								rawData.indices.push_back(buffer[k]);
							break;
						}
						case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
							const std::uint16_t* buffer = (const std::uint16_t*)dataPtr;
							for (std::size_t k = 0; k < indicesAccessor.componentType; k++)
								rawData.indices.push_back(buffer[k]);
							break;
						}
						case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
							const std::uint8_t* buffer = (const std::uint8_t*)dataPtr;
							for (std::size_t k = 0; k < indicesAccessor.componentType; k++)
								rawData.indices.push_back(buffer[k]);
							break;
						}
						default:
							throw Utils::Error("Index component type %d not supported.\n", indicesAccessor.componentType);
							return;
					}
				}

				vk::Primitive* newPrimitive = new vk::Primitive(rawData.indices.size(), indexCount, vertexCount, primitive.material > -1 ? vkModel.materials[primitive.material] : vkModel.materials.back());
				newMesh->primitives.push_back(newPrimitive);

			}
		}

		std::fprintf(stdout, "After primitive loop\n");

		if (parent)
			parent->children.push_back(newNode);
		else
			vkModel.nodes.push_back(newNode);
	}

	void createVulkanBuffers(vk::Model& vkModel, vk::RawData& rawData) {
		std::fprintf(stdout, "positions: %zu\nnormals: %zu\ntexCoords: %zu\ncolours: %zu\n", rawData.positions.size(), rawData.normals.size(), rawData.texCoords.size(), rawData.vertexColours.size());
	}

}
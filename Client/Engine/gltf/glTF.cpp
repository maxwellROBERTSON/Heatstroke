#include "glTF.hpp"

#include <glm/gtc/type_ptr.inl>

#include "Error.hpp"
#include "toString.hpp"
#include "VulkanEnums.hpp"
#include "VulkanUtils.hpp"
#include "../vulkan/VulkanDevice.hpp"
#include "../vulkan/objects/Buffer.hpp"

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
		loadTextureSamplers(*aContext.window, model, vkModel);

		// 3. Load textures
		loadTextures(aContext, model, vkModel);

		// 4. Determine default scene
		const tinygltf::Scene& scene = model.scenes[model.defaultScene > -1 ? model.defaultScene : 0];

		// 5. Recurse through scene nodes and get mesh data for each

		std::uint32_t indicesCount = 0, verticesCount = 0;

		// Get indices and vertices count for all nodes in the scene
		for (std::size_t i = 0; i < model.nodes.size(); i++)
			getCounts(model, model.nodes[i], indicesCount, verticesCount);

		vk::RawData rawData(verticesCount, indicesCount);

		// Load the node meshes into Vulkan objects
		for (std::size_t i = 0; i < model.nodes.size(); i++) {
			tinygltf::Node node = model.nodes[i];
			loadNodeMeshes(nullptr, node, model, i, rawData, vkModel);
		}

		createVulkanBuffers(aContext, vkModel, rawData);

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
			vkMaterial.emissiveStrength = 1.0f; // This can be changed using a glTF extension, which we may want to account for

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
			
			vkMaterial.index = vkModel.materials.size();

			vkModel.materials.emplace_back(vkMaterial);
		}
	}

	void loadTextureSamplers(const VulkanWindow& aWindow, tinygltf::Model& model, vk::Model& vkModel) {
		vk::SamplerInfo samplerInfo;
		samplerInfo.magFilter = VK_FILTER_NEAREST;
		samplerInfo.minFilter = VK_FILTER_NEAREST;
		samplerInfo.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
		samplerInfo.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;

		vkModel.defaultSampler = createTextureSampler(aWindow, samplerInfo);

		for (tinygltf::Sampler sampler : model.samplers) {
			vk::SamplerInfo vkSampler{};
			vkSampler.minFilter = Utils::getVkFilter(sampler.minFilter);
			vkSampler.magFilter = Utils::getVkFilter(sampler.magFilter);
			vkSampler.addressModeU = Utils::getVkSamplerAddressMode(sampler.wrapS);
			vkSampler.addressModeV = Utils::getVkSamplerAddressMode(sampler.wrapT);
			
			vkModel.samplers.emplace_back(createTextureSampler(aWindow, vkSampler));
		}
	}

	void loadTextures(const VulkanContext& aContext, tinygltf::Model& model, vk::Model& vkModel) {
		for (std::size_t i = 0; i < model.textures.size(); i++) {
			tinygltf::Texture texture = model.textures[i];

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

			VkSampler sampler = texture.sampler > -1 ? vkModel.samplers[texture.sampler].handle : vkModel.defaultSampler.handle;

			vk::Texture vkTexture = vk::createTexture(aContext, image, format, sampler);
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

		if (!node.children.empty()) {
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
				const float* texCoords0Pos = nullptr;
				const float* texCoords1Pos = nullptr;
				const float* colourPos = nullptr;
				int positionByteStride;
				int normalsByteStride;
				int texCoords0ByteStride;
				int texCoords1ByteStride;
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
					const tinygltf::Accessor& texCoord0Accessor = model.accessors[primitive.attributes.find("TEXCOORD_0")->second];
					const tinygltf::BufferView& texCoord0BufView = model.bufferViews[texCoord0Accessor.bufferView];
					texCoords0Pos = reinterpret_cast<const float*>(&(model.buffers[texCoord0BufView.buffer].data[texCoord0Accessor.byteOffset + texCoord0BufView.byteOffset]));
					texCoords0ByteStride = texCoord0Accessor.ByteStride(texCoord0BufView) ? (texCoord0Accessor.ByteStride(texCoord0BufView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
				}

				if (primitive.attributes.find("TEXCOORD_1") != primitive.attributes.end()) {
					const tinygltf::Accessor& texCoord1Accessor = model.accessors[primitive.attributes.find("TEXCOORD_1")->second];
					const tinygltf::BufferView& texCoord1BufView = model.bufferViews[texCoord1Accessor.bufferView];
					texCoords1Pos = reinterpret_cast<const float*>(&(model.buffers[texCoord1BufView.buffer].data[texCoord1Accessor.byteOffset + texCoord1BufView.byteOffset]));
					texCoords1ByteStride = texCoord1Accessor.ByteStride(texCoord1BufView) ? (texCoord1Accessor.ByteStride(texCoord1BufView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC2);
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
					rawData.texCoords0.emplace_back(texCoords0Pos ? glm::make_vec2(&texCoords0Pos[j * texCoords0ByteStride]) : glm::vec2(0.0f));
					rawData.texCoords1.emplace_back(texCoords1Pos ? glm::make_vec2(&texCoords1Pos[j * texCoords1ByteStride]) : glm::vec2(0.0f));
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
							for (std::size_t k = 0; k < indicesAccessor.count; k++)
								rawData.indices.push_back(buffer[k]);
							vkModel.indexType = VK_INDEX_TYPE_UINT32;
							break;
						}
						case TINYGLTF_PARAMETER_TYPE_UNSIGNED_SHORT: {
							const std::uint16_t* buffer = (const std::uint16_t*)dataPtr;
							for (std::size_t k = 0; k < indicesAccessor.count; k++)
								rawData.indices.push_back(buffer[k]);
							vkModel.indexType = VK_INDEX_TYPE_UINT16;
							break;
						}
						case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
							const std::uint8_t* buffer = (const std::uint8_t*)dataPtr;
							for (std::size_t k = 0; k < indicesAccessor.count; k++)
								rawData.indices.push_back(buffer[k]);
							break;
						}
						default:
							throw Utils::Error("Index component type %d not supported.\n", indicesAccessor.componentType);
							return;
					}
				}

				vk::Primitive* newPrimitive = new vk::Primitive(0, indexCount, vertexCount, primitive.material > -1 ? vkModel.materials[primitive.material] : vkModel.materials.back());
				newMesh->primitives.push_back(newPrimitive);

			}
			newNode->mesh = newMesh;
		}

		if (parent)
			parent->children.push_back(newNode);
		else
			vkModel.nodes.push_back(newNode);
	}

	void createVulkanBuffers(const VulkanContext& aContext, vk::Model& vkModel, vk::RawData& rawData) {
		// Pre-calculate sizes for less code duplication
		std::size_t posSize = rawData.positions.size() * sizeof(glm::vec3);
		std::size_t normSize = rawData.normals.size() * sizeof(glm::vec3);
		std::size_t tex0Size = rawData.texCoords0.size() * sizeof(glm::vec2);
		std::size_t tex1Size = rawData.texCoords1.size() * sizeof(glm::vec2);
		std::size_t vertColSize = rawData.vertexColours.size() * sizeof(glm::vec4);
		std::size_t indicesSize = rawData.indices.size() * (vkModel.indexType == VK_INDEX_TYPE_UINT32 ? sizeof(std::uint32_t) : sizeof(std::uint32_t));

		// GPU sided buffers
		vk::Buffer posGPUBuf = vk::createBuffer(
			*aContext.allocator,
			posSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
		);

		vk::Buffer normGPUBuf = vk::createBuffer(
			*aContext.allocator,
			normSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
		);

		vk::Buffer tex0GPUBuf = vk::createBuffer(
			*aContext.allocator,
			tex0Size,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
		);

		vk::Buffer tex1GPUBuf = vk::createBuffer(
			*aContext.allocator,
			tex1Size,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
		);

		vk::Buffer vertColGPUBuf = vk::createBuffer(
			*aContext.allocator,
			vertColSize,
			VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
		);

		vk::Buffer indicesGPUBuf = vk::createBuffer(
			*aContext.allocator,
			indicesSize,
			VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
			0,
			VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
		);

		// Staging buffers
		vk::Buffer posStaging = vk::createBuffer(
			*aContext.allocator,
			posSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		);

		vk::Buffer normStaging = vk::createBuffer(
			*aContext.allocator,
			normSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		);

		vk::Buffer tex0Staging = vk::createBuffer(
			*aContext.allocator,
			tex0Size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		);

		vk::Buffer tex1Staging = vk::createBuffer(
			*aContext.allocator,
			tex1Size,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		);

		vk::Buffer vertColStaging = vk::createBuffer(
			*aContext.allocator,
			vertColSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		);

		vk::Buffer indicesStaging = vk::createBuffer(
			*aContext.allocator,
			indicesSize,
			VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
			VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
		);

		// Copy to ptr
		void* posPtr = nullptr;
		if (const auto res = vmaMapMemory(aContext.allocator->allocator, posStaging.allocation, &posPtr); VK_SUCCESS != res)
			throw Utils::Error("Mapping memory for writing\n vmaMapMemory() returned %s", Utils::toString(res).c_str());

		std::memcpy(posPtr, rawData.positions.data(), posSize);
		vmaUnmapMemory(aContext.allocator->allocator, posStaging.allocation);

		void* normPtr = nullptr;
		if (const auto res = vmaMapMemory(aContext.allocator->allocator, normStaging.allocation, &normPtr); VK_SUCCESS != res)
			throw Utils::Error("Mapping memory for writing\n vmaMapMemory() returned %s", Utils::toString(res).c_str());

		std::memcpy(normPtr, rawData.normals.data(), normSize);
		vmaUnmapMemory(aContext.allocator->allocator, normStaging.allocation);

		void* tex0Ptr = nullptr;
		if (const auto res = vmaMapMemory(aContext.allocator->allocator, tex0Staging.allocation, &tex0Ptr); VK_SUCCESS != res)
			throw Utils::Error("Mapping memory for writing\n vmaMapMemory() returned %s", Utils::toString(res).c_str());

		std::memcpy(tex0Ptr, rawData.texCoords0.data(), tex0Size);
		vmaUnmapMemory(aContext.allocator->allocator, tex0Staging.allocation);

		void* tex1Ptr = nullptr;
		if (const auto res = vmaMapMemory(aContext.allocator->allocator, tex1Staging.allocation, &tex1Ptr); VK_SUCCESS != res)
			throw Utils::Error("Mapping memory for writing\n vmaMapMemory() returned %s", Utils::toString(res).c_str());

		std::memcpy(tex1Ptr, rawData.texCoords1.data(), tex1Size);
		vmaUnmapMemory(aContext.allocator->allocator, tex1Staging.allocation);

		void* vertColPtr = nullptr;
		if (const auto res = vmaMapMemory(aContext.allocator->allocator, vertColStaging.allocation, &vertColPtr); VK_SUCCESS != res)
			throw Utils::Error("Mapping memory for writing\n vmaMapMemory() returned %s", Utils::toString(res).c_str());

		std::memcpy(vertColPtr, rawData.vertexColours.data(), vertColSize);
		vmaUnmapMemory(aContext.allocator->allocator, vertColStaging.allocation);

		void* indicesPtr = nullptr;
		if (const auto res = vmaMapMemory(aContext.allocator->allocator, indicesStaging.allocation, &indicesPtr); VK_SUCCESS != res)
			throw Utils::Error("Mapping memory for writing\n vmaMapMemory() returned %s", Utils::toString(res).c_str());

		std::memcpy(indicesPtr, rawData.indices.data(), indicesSize);
		vmaUnmapMemory(aContext.allocator->allocator, indicesStaging.allocation);
		
		vk::Fence uploadComplete = createFence(*aContext.window);

		VkCommandBuffer uploadCmdBuf = createCommandBuffer(*aContext.window);

		beginCommandBuffer(uploadCmdBuf);

		VkBufferCopy posCopy{};
		posCopy.size = posSize;

		vkCmdCopyBuffer(uploadCmdBuf, posStaging.buffer, posGPUBuf.buffer, 1, &posCopy);

		Utils::bufferBarrier(
			uploadCmdBuf,
			posGPUBuf.buffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
		);

		VkBufferCopy normCopy{};
		normCopy.size = normSize;

		vkCmdCopyBuffer(uploadCmdBuf, normStaging.buffer, normGPUBuf.buffer, 1, &normCopy);

		Utils::bufferBarrier(
			uploadCmdBuf,
			normGPUBuf.buffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
		);

		VkBufferCopy tex0Copy{};
		tex0Copy.size = tex0Size;

		vkCmdCopyBuffer(uploadCmdBuf, tex0Staging.buffer, tex0GPUBuf.buffer, 1, &tex0Copy);

		Utils::bufferBarrier(
			uploadCmdBuf,
			tex0GPUBuf.buffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
		);

		VkBufferCopy tex1Copy{};
		tex1Copy.size = tex1Size;

		vkCmdCopyBuffer(uploadCmdBuf, tex1Staging.buffer, tex1GPUBuf.buffer, 1, &tex1Copy);

		Utils::bufferBarrier(
			uploadCmdBuf,
			tex1GPUBuf.buffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
		);

		VkBufferCopy vertColCopy{};
		vertColCopy.size = vertColSize;

		vkCmdCopyBuffer(uploadCmdBuf, vertColStaging.buffer, vertColGPUBuf.buffer, 1, &vertColCopy);

		Utils::bufferBarrier(
			uploadCmdBuf,
			vertColGPUBuf.buffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
		);

		VkBufferCopy indicesCopy{};
		indicesCopy.size = indicesSize;

		vkCmdCopyBuffer(uploadCmdBuf, indicesStaging.buffer, indicesGPUBuf.buffer, 1, &indicesCopy);

		Utils::bufferBarrier(
			uploadCmdBuf,
			indicesGPUBuf.buffer,
			VK_ACCESS_TRANSFER_WRITE_BIT,
			VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
			VK_PIPELINE_STAGE_TRANSFER_BIT,
			VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
		);

		endAndSubmitCommandBuffer(*aContext.window, uploadCmdBuf);

		vkModel.posBuffer = std::move(posGPUBuf);
		vkModel.normBuffer = std::move(normGPUBuf);
		vkModel.tex0Buffer = std::move(tex0GPUBuf);
		vkModel.tex1Buffer = std::move(tex1GPUBuf);
		vkModel.vertColBuffer = std::move(vertColGPUBuf);
		vkModel.indicesBuffer = std::move(indicesGPUBuf);
	}

}
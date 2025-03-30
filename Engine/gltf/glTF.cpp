#include "glTF.hpp"

#include <glm/gtc/type_ptr.inl>
#include <tgen.h>

#include "Error.hpp"
#include "toString.hpp"
#include "Animation.hpp"
#include "VulkanEnums.hpp"
#include "VulkanUtils.hpp"
#include "../vulkan/VulkanDevice.hpp"
#include "../vulkan/objects/Buffer.hpp"

#define MAX_JOINTS 128u

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
		// Load the node meshes into Vulkan objects
		for (std::size_t i = 0; i < model.nodes.size(); i++) {
			tinygltf::Node node = model.nodes[i];
			loadNodeMeshes(nullptr, node, model, i, vkModel);
		}

		// 6. Calculate bounding box of entire model
		calculateModelBoundingBox(vkModel);

		// 7. Load animation data
		loadAnimations(model, vkModel);

		// 8. Load skin data
		loadSkins(model, vkModel);

		createVulkanBuffers(aContext, vkModel);

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
		// Create dummy texture for materials that are missing some textures
		vkModel.dummyTexture = vk::createDummyTexture(aContext, vkModel.defaultSampler.handle);
		vkModel.dummyImageView = vk::createImageView(*aContext.window, vkModel.dummyTexture.image, VK_FORMAT_R8G8B8A8_UNORM);

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

			vk::Texture vkTexture = vk::createTexture(aContext, "glTF texture", image, format, sampler);
			vk::ImageView vkImageView = vk::createImageView(*aContext.window, vkTexture.image, format);

			vkModel.textures.emplace_back(std::move(vkTexture));
			vkModel.imageViews.emplace_back(std::move(vkImageView));
		}
	}

	void loadNodeMeshes(vk::Node* parent, tinygltf::Node& node, tinygltf::Model& model, std::uint32_t nodeIndex, vk::Model& vkModel) {
		vk::Node* newNode = new vk::Node();
		newNode->index = nodeIndex;
		newNode->skinIndex = node.skin;
		newNode->parent = parent;
		newNode->nodeMatrix = glm::mat4(1.0f);

		bool hasMatrix = false;

		// glTF spec states that nodes will define a local space transform using either
		// a matrix OR TRS properties, meaning they are mutually exclusive for a single node.
		if (node.matrix.size() == 16) {
			newNode->nodeMatrix = glm::make_mat4x4(node.matrix.data());
			hasMatrix = true;
		}

		if (node.translation.size() == 3)
			newNode->translation = glm::make_vec3(node.translation.data());

		if (node.rotation.size() == 4)
			newNode->rotation = glm::make_quat(node.rotation.data());

		if (node.scale.size() == 3)
			newNode->scale = glm::make_vec3(node.scale.data());

		if (node.mesh > -1) {
			const tinygltf::Mesh mesh = model.meshes[node.mesh];
			vk::Mesh* newMesh = new vk::Mesh(newNode->nodeMatrix);

			vkModel.meshedNodes++;

			for (std::size_t i = 0; i < mesh.primitives.size(); i++) {
				const tinygltf::Primitive& primitive = mesh.primitives[i];

				glm::vec3 bbMin;
				glm::vec3 bbMax;
				std::uint32_t indexCount = 0, vertexCount = 0;
				bool hasIndices = primitive.indices > -1;
				bool getTangentsFromTgen = false;

				const float* bufferPos = nullptr;
				const float* normalsPos = nullptr;
				const float* tangentPos = nullptr;
				const float* texCoords0Pos = nullptr;
				const float* texCoords1Pos = nullptr;
				const float* colourPos = nullptr;
				const void* jointsPos = nullptr;
				const float* weightsPos = nullptr;

				int positionByteStride;
				int normalsByteStride;
				int tangentByteStride;
				int texCoords0ByteStride;
				int texCoords1ByteStride;
				int colourByteStride;
				int jointByteStride;
				int weightByteStride;

				const tinygltf::Accessor& positionAccessor = model.accessors[primitive.attributes.find("POSITION")->second];
				const tinygltf::BufferView& positionBufView = model.bufferViews[positionAccessor.bufferView];
				bufferPos = reinterpret_cast<const float*>(&(model.buffers[positionBufView.buffer].data[positionAccessor.byteOffset + positionBufView.byteOffset]));
				bbMin = glm::vec3(positionAccessor.minValues[0], positionAccessor.minValues[1], positionAccessor.minValues[2]);
				bbMax = glm::vec3(positionAccessor.maxValues[0], positionAccessor.maxValues[1], positionAccessor.maxValues[2]);
				vertexCount = positionAccessor.count;
				positionByteStride = positionAccessor.ByteStride(positionBufView) ? (positionAccessor.ByteStride(positionBufView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);

				// Get normals
				if (primitive.attributes.find("NORMAL") != primitive.attributes.end()) {
					const tinygltf::Accessor& normalAccessor = model.accessors[primitive.attributes.find("NORMAL")->second];
					const tinygltf::BufferView& normalBufView = model.bufferViews[normalAccessor.bufferView];
					normalsPos = reinterpret_cast<const float*>(&(model.buffers[normalBufView.buffer].data[normalAccessor.byteOffset + normalBufView.byteOffset]));
					normalsByteStride = normalAccessor.ByteStride(normalBufView) ? (normalAccessor.ByteStride(normalBufView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC3);
				}

				// Get tangents
				if (primitive.attributes.find("TANGENT") != primitive.attributes.end()) {
					const tinygltf::Accessor& tangentAccessor = model.accessors[primitive.attributes.find("TANGENT")->second];
					const tinygltf::BufferView& tangentBufView = model.bufferViews[tangentAccessor.bufferView];
					tangentPos = reinterpret_cast<const float*>(&(model.buffers[tangentBufView.buffer].data[tangentAccessor.byteOffset + tangentBufView.byteOffset]));
					tangentByteStride = tangentAccessor.ByteStride(tangentBufView) ? (tangentAccessor.ByteStride(tangentBufView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
				}
				else {
					getTangentsFromTgen = true;
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
					colourByteStride = colourAccessor.ByteStride(colourBufView) ? (colourAccessor.ByteStride(colourBufView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
				}

				// Get joints
				int jointComponentType;

				if (primitive.attributes.find("JOINTS_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& jointsAccessor = model.accessors[primitive.attributes.find("JOINTS_0")->second];
					const tinygltf::BufferView& jointsBufView = model.bufferViews[jointsAccessor.bufferView];
					jointsPos = &(model.buffers[jointsBufView.buffer].data[jointsAccessor.byteOffset + jointsBufView.byteOffset]);
					jointComponentType = jointsAccessor.componentType;
					jointByteStride = jointsAccessor.ByteStride(jointsBufView) ? (jointsAccessor.ByteStride(jointsBufView) / tinygltf::GetComponentSizeInBytes(jointComponentType)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
				}

				// Get weights
				if (primitive.attributes.find("WEIGHTS_0") != primitive.attributes.end()) {
					const tinygltf::Accessor& weightsAccessor = model.accessors[primitive.attributes.find("WEIGHTS_0")->second];
					const tinygltf::BufferView& weightsBufView = model.bufferViews[weightsAccessor.bufferView];
					weightsPos = reinterpret_cast<const float*>(&(model.buffers[weightsBufView.buffer].data[weightsAccessor.byteOffset + weightsBufView.byteOffset]));
					weightByteStride = weightsAccessor.ByteStride(weightsBufView) ? (weightsAccessor.ByteStride(weightsBufView) / sizeof(float)) : tinygltf::GetNumComponentsInType(TINYGLTF_TYPE_VEC4);
				}

				if (!hasIndices)
					throw Utils::Error("Warning: A glTF primitive detected to not have any indices. We have no compatability for index-less meshes!\n");

				const tinygltf::Accessor& indicesAccessor = model.accessors[primitive.indices > -1 ? primitive.indices : 0];
				const tinygltf::BufferView& indicesBufferView = model.bufferViews[indicesAccessor.bufferView];
				const tinygltf::Buffer& indicesBuffer = model.buffers[indicesBufferView.buffer];

				indexCount = indicesAccessor.count;
				bool hasSkin = (jointsPos && weightsPos);

				// Reserve space in rawData struct
				vk::RawData rawData(vertexCount, indexCount);

				// Populate per-vertex attributes
				for (std::size_t j = 0; j < vertexCount; j++) {
					rawData.positions.emplace_back(glm::make_vec3(&bufferPos[j * positionByteStride]));
					rawData.normals.emplace_back(glm::normalize(glm::vec3(normalsPos ? glm::make_vec3(&normalsPos[j * normalsByteStride]) : glm::vec3(0.0f))));
					rawData.tangents.emplace_back(glm::vec4(tangentPos ? glm::make_vec4(&tangentPos[j * tangentByteStride]) : glm::vec4(0.0f)));
					rawData.texCoords0.emplace_back(texCoords0Pos ? glm::make_vec2(&texCoords0Pos[j * texCoords0ByteStride]) : glm::vec2(0.0f));
					rawData.texCoords1.emplace_back(texCoords1Pos ? glm::make_vec2(&texCoords1Pos[j * texCoords1ByteStride]) : glm::vec2(0.0f));
					rawData.vertexColours.emplace_back(colourPos ? glm::make_vec4(&colourPos[j * colourByteStride]) : glm::vec4(1.0f));

					if (hasSkin) {
						switch (jointComponentType) {
						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_SHORT: {
							const std::uint16_t* buffer = (const std::uint16_t*)jointsPos;
							rawData.joints.push_back(glm::uvec4(glm::make_vec4(&buffer[j * jointByteStride])));
							break;
						}
						case TINYGLTF_COMPONENT_TYPE_UNSIGNED_BYTE: {
							const std::uint8_t* buffer = (const std::uint8_t*)jointsPos;
							rawData.joints.push_back(glm::vec4(glm::make_vec4(&buffer[j * jointByteStride])));
							break;
						}
						default:
							throw Utils::Error("Joint component type %d not supported.\n", jointComponentType);
							return;
						}
					}
					else {
						rawData.joints.push_back(glm::vec4(0.0f));
					}

					rawData.weights.push_back(hasSkin ? glm::make_vec4(&weightsPos[j * weightByteStride]) : glm::vec4(1.0f));
				}

				const void* dataPtr = &(indicesBuffer.data[indicesAccessor.byteOffset + indicesBufferView.byteOffset]);

				// Populate indices
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
				// 1 byte indices require an extension for Vulkan to correctly handle them
				//case TINYGLTF_PARAMETER_TYPE_UNSIGNED_BYTE: {
				//	const std::uint8_t* buffer = (const std::uint8_t*)dataPtr;
				//	for (std::size_t k = 0; k < indicesAccessor.count; k++)
				//		rawData.indices.push_back(buffer[k]);
				//	break;
				//}
				default:
					throw Utils::Error("Index component type %d not supported.\n", indicesAccessor.componentType);
					return;
				}

				if (getTangentsFromTgen) {
					std::vector<tgen::VIndexT> newIndices(rawData.indices.begin(), rawData.indices.end());

					std::vector<tgen::RealT> tgenVertices, tgenTexCoords, tgenNormals;

					for (glm::vec3 vertex : rawData.positions) {
						tgenVertices.push_back(vertex.x);
						tgenVertices.push_back(vertex.y);
						tgenVertices.push_back(vertex.z);
					}

					// Hopefully the tex coords tgen needs are the ones from texCoords0
					for (glm::vec2 texCoord : rawData.texCoords0) {
						tgenTexCoords.push_back(texCoord.x);
						tgenTexCoords.push_back(texCoord.y);
					}

					for (glm::vec3 normal : rawData.normals) {
						tgenNormals.push_back(normal.x);
						tgenNormals.push_back(normal.y);
						tgenNormals.push_back(normal.z);
					}

					// Tangent and Bitangent destination vectors
					std::vector<tgen::RealT> cornerTangents, cornerBitangents;
					std::vector<tgen::RealT> vertexTangents, vertexBitangents;

					// Final tangent result vector
					std::vector<tgen::RealT> tangents;

					tgen::computeCornerTSpace(newIndices, newIndices, tgenVertices, tgenTexCoords, cornerTangents, cornerBitangents);
					tgen::computeVertexTSpace(newIndices, cornerTangents, cornerBitangents, newIndices.size(), vertexTangents, vertexBitangents);
					tgen::orthogonalizeTSpace(tgenNormals, vertexTangents, vertexBitangents);
					tgen::computeTangent4D(tgenNormals, vertexTangents, vertexBitangents, tangents);

					for (std::size_t l = 0, m = 0; l < tangents.size(); l += 4, m++)
						rawData.tangents[m] = glm::vec4(tangents[l], tangents[l + 1], tangents[l + 2], tangents[l + 3]);
				}

				vk::Material& material = primitive.material > -1 ? vkModel.materials[primitive.material] : vkModel.materials.back();
				vk::Primitive* newPrimitive = new vk::Primitive(0, indexCount, vertexCount, rawData, material);

				// Scale bounding box for this node based on given transform properties
				glm::vec3 scale = hasMatrix ? glm::vec3(newNode->nodeMatrix[0][0], newNode->nodeMatrix[1][1], newNode->nodeMatrix[2][2]) : newNode->scale;
				bbMin *= scale;
				bbMax *= scale;
				newPrimitive->setBounds(bbMin, bbMax);
				newMesh->primitives.push_back(newPrimitive);
			}

			newNode->mesh = newMesh;

			// Calculate bounding box for entire node
			for (vk::Primitive* primitive : newNode->mesh->primitives) {
				// Get minimum bound for this node
				if (primitive->min.x < newNode->bbMin.x)
					newNode->bbMin.x = primitive->min.x;
				if (primitive->min.y < newNode->bbMin.y)
					newNode->bbMin.y = primitive->min.y;
				if (primitive->min.z < newNode->bbMin.z)
					newNode->bbMin.z = primitive->min.z;
				// Get maximum bound for this node
				if (primitive->max.x > newNode->bbMax.x)
					newNode->bbMax.x = primitive->max.x;
				if (primitive->max.y > newNode->bbMax.y)
					newNode->bbMax.y = primitive->max.y;
				if (primitive->max.z > newNode->bbMax.z)
					newNode->bbMax.z = primitive->max.z;
			}
		}

		if (parent)
			parent->children.push_back(newNode);
		else
			vkModel.nodes.push_back(newNode);
	}

	void calculateModelBoundingBox(vk::Model& vkModel) {
		for (vk::Node* node : vkModel.nodes) {
			if (node->bbMin.x < vkModel.bbMin.x)
				vkModel.bbMin.x = node->bbMin.x;
			if (node->bbMin.y < vkModel.bbMin.y)
				vkModel.bbMin.y = node->bbMin.y;
			if (node->bbMin.z < vkModel.bbMin.z)
				vkModel.bbMin.z = node->bbMin.z;

			if (node->bbMax.x > vkModel.bbMax.x)
				vkModel.bbMax.x = node->bbMax.x;
			if (node->bbMax.y > vkModel.bbMax.y)
				vkModel.bbMax.y = node->bbMax.y;
			if (node->bbMax.z > vkModel.bbMax.z)
				vkModel.bbMax.z = node->bbMax.z;
		}
	}

	void loadAnimations(tinygltf::Model& model, vk::Model& vkModel) {
		if (model.animations.size() == 0)
			return;

		for (tinygltf::Animation& gltfAnimation : model.animations) {
			vk::Animation animation{};
			
			animation.name = gltfAnimation.name;
			if (animation.name.empty())
				animation.name = std::to_string(vkModel.animations.size());

			// Parse samplers
			for (tinygltf::AnimationSampler& animSampler : gltfAnimation.samplers) {
				vk::AnimationSampler sampler{};
			
				if (animSampler.interpolation == "LINEAR")
					sampler.interpolationType = vk::AnimationSampler::InterpolationType::LINEAR;
				if (animSampler.interpolation == "STEP")
					sampler.interpolationType = vk::AnimationSampler::InterpolationType::STEP;
				if (animSampler.interpolation == "CUBICSPLINE")
					sampler.interpolationType = vk::AnimationSampler::InterpolationType::CUBIC_SPLINE;

				// Parse sampler inputs
				{
					const tinygltf::Accessor& accessor = model.accessors[animSampler.input];
					const tinygltf::BufferView& bufView = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[bufView.buffer];

					const void* dataPtr = &buffer.data[accessor.byteOffset + bufView.byteOffset];
					const float* bufValues = (const float*)dataPtr;

					for (std::size_t i = 0; i < accessor.count; i++) {
						sampler.keyframeTimes.push_back(bufValues[i]);
					}

					for (float time : sampler.keyframeTimes) {
						if (time < animation.start)
							animation.start = time;
						if (time > animation.end)
							animation.end = time;
					}
				}

				// Parse sampler outputs
				{
					const tinygltf::Accessor& accessor = model.accessors[animSampler.output];
					const tinygltf::BufferView& bufView = model.bufferViews[accessor.bufferView];
					const tinygltf::Buffer& buffer = model.buffers[bufView.buffer];
				
					const void* dataPtr = &buffer.data[accessor.byteOffset + bufView.byteOffset];

					switch (accessor.type) {
					case TINYGLTF_TYPE_VEC3: {
						const glm::vec3* buf = (const glm::vec3*)dataPtr;
						
						for (std::size_t i = 0; i < accessor.count; i++) {
							sampler.outputValues.push_back(glm::vec4(buf[i], 0.0f));
						}

						break;
					}
					case TINYGLTF_TYPE_VEC4: {
						const glm::vec4* buf = (const glm::vec4*)dataPtr;

						for (std::size_t i = 0; i < accessor.count; i++) {
							sampler.outputValues.push_back(glm::vec4(buf[i]));
						}

						break;
					}
					default:
						std::cout << "Unknown channel type!" << std::endl;
						break;
					}
				}

				animation.samplers.push_back(sampler);
			}

			// Parse channels
			for (tinygltf::AnimationChannel& animChannel : gltfAnimation.channels) {
				vk::AnimationChannel channel{};

				if (animChannel.target_path == "translation")
					channel.pathType = vk::AnimationChannel::PathType::TRANSLATION;
				if (animChannel.target_path == "rotation")
					channel.pathType = vk::AnimationChannel::PathType::ROTATION;
				if (animChannel.target_path == "scale")
					channel.pathType = vk::AnimationChannel::PathType::SCALE;
				if (animChannel.target_path == "weights") {
					std::cout << "AnimationChannel 'weights' not supported!" << std::endl;
					continue;
				}

				channel.samplerIndex = animChannel.sampler;
				channel.node = vkModel.getNodeFromIndex(animChannel.target_node);
				
				if (!channel.node)
					continue;
			
				animation.channels.push_back(channel);
			}

			vkModel.animations.push_back(animation);
		}
	}

	void loadSkins(tinygltf::Model& model, vk::Model& vkModel) {
		for (tinygltf::Skin& gltfSkin : model.skins) {
			vk::Skin skin{};

			skin.name = gltfSkin.name;

			// Get skeleton root node
			if (gltfSkin.skeleton > -1)
				skin.rootNode = vkModel.getNodeFromIndex(gltfSkin.skeleton);

			// Get inverse bind matrices
			if (gltfSkin.inverseBindMatrices > -1) {
				const tinygltf::Accessor& accessor = model.accessors[gltfSkin.inverseBindMatrices];
				const tinygltf::BufferView& bufView = model.bufferViews[accessor.bufferView];
				const tinygltf::Buffer& buffer = model.buffers[bufView.buffer];

				skin.inverseBindMatrices.resize(accessor.count);
				std::memcpy(skin.inverseBindMatrices.data(), &buffer.data[accessor.byteOffset + bufView.byteOffset], accessor.count * sizeof(glm::mat4));
			}

			// Get joints
			for (int joint : gltfSkin.joints) {
				vk::Node* node = vkModel.getNodeFromIndex(joint);

				if (node)
					skin.joints.push_back(node);
			}

			if (skin.joints.size() > MAX_JOINTS) {
				std::cout << "Warning: Number of joints (" << skin.joints.size() << ") exceeds the supported maximum: " << MAX_JOINTS << "." << std::endl;
			}

			vkModel.skins.push_back(skin);
		}

		// Give nodes their respective skin
		for (vk::Node* node : vkModel.nodes) {
			if (node->skinIndex > -1)
				node->skin = &vkModel.skins[node->skinIndex];
		}
	}

	void createVulkanBuffers(const VulkanContext& aContext, vk::Model& vkModel) {
		// We need to create the buffers for every vertex attribute and indices for every primitive
		for (vk::Node* node : vkModel.nodes) {

			if (!node->mesh) continue;

			for (vk::Primitive* primitive : node->mesh->primitives) {
				vk::RawData& rawData = primitive->rawData;

				// Pre-calculate sizes for less code duplication
				std::size_t posSize = rawData.positions.size() * sizeof(glm::vec3);
				std::size_t normSize = rawData.normals.size() * sizeof(glm::vec3);
				std::size_t tangentSize = rawData.tangents.size() * sizeof(glm::vec4);
				std::size_t tex0Size = rawData.texCoords0.size() * sizeof(glm::vec2);
				std::size_t tex1Size = rawData.texCoords1.size() * sizeof(glm::vec2);
				std::size_t vertColSize = rawData.vertexColours.size() * sizeof(glm::vec4);
				std::size_t jointsSize = rawData.joints.size() * sizeof(glm::uvec4);
				std::size_t weightsSize = rawData.weights.size() * sizeof(glm::vec4);
				std::size_t indicesSize = rawData.indices.size() * (vkModel.indexType == VK_INDEX_TYPE_UINT32 ? sizeof(std::uint32_t) : sizeof(std::uint32_t));

				// GPU sided buffers
				vk::Buffer posGPUBuf = vk::createBuffer(
					"posGPU",
					*aContext.allocator,
					posSize,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					0,
					VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
				);

				vk::Buffer normGPUBuf = vk::createBuffer(
					"normGPU",
					*aContext.allocator,
					normSize,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					0,
					VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
				);

				vk::Buffer tangentGPUBuf = vk::createBuffer(
					"tangGPU",
					*aContext.allocator,
					tangentSize,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					0,
					VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
				);

				vk::Buffer tex0GPUBuf = vk::createBuffer(
					"tex0GPU",
					*aContext.allocator,
					tex0Size,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					0,
					VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
				);

				vk::Buffer tex1GPUBuf = vk::createBuffer(
					"tex1GPU",
					*aContext.allocator,
					tex1Size,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					0,
					VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
				);

				vk::Buffer vertColGPUBuf = vk::createBuffer(
					"vertColGPU",
					*aContext.allocator,
					vertColSize,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					0,
					VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
				);

				vk::Buffer jointsGPUBuf = vk::createBuffer(
					"jointsGPU",
					*aContext.allocator,
					jointsSize,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					0,
					VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
				);

				vk::Buffer weightsGPUBuf = vk::createBuffer(
					"weightsGPU",
					*aContext.allocator,
					weightsSize,
					VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					0,
					VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
				);

				vk::Buffer indicesGPUBuf = vk::createBuffer(
					"indicesGPU",
					*aContext.allocator,
					indicesSize,
					VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
					0,
					VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
				);

				// Staging buffers
				vk::Buffer posStaging = vk::createBuffer(
					"posStaging",
					*aContext.allocator,
					posSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
				);

				vk::Buffer normStaging = vk::createBuffer(
					"normStaging",
					*aContext.allocator,
					normSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
				);

				vk::Buffer tangentStaging = vk::createBuffer(
					"tangStaging",
					*aContext.allocator,
					tangentSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
				);

				vk::Buffer tex0Staging = vk::createBuffer(
					"tex0Staging",
					*aContext.allocator,
					tex0Size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
				);

				vk::Buffer tex1Staging = vk::createBuffer(
					"tex1Staging",
					*aContext.allocator,
					tex1Size,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
				);

				vk::Buffer vertColStaging = vk::createBuffer(
					"vertColStaging",
					*aContext.allocator,
					vertColSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
				);

				vk::Buffer jointsStaging = vk::createBuffer(
					"jointsStaging",
					*aContext.allocator,
					jointsSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
				);

				vk::Buffer weightsStaging = vk::createBuffer(
					"weightsStaging",
					*aContext.allocator,
					weightsSize,
					VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
				);

				vk::Buffer indicesStaging = vk::createBuffer(
					"indicesStaging",
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

				void* tangentPtr = nullptr;
				if (const auto res = vmaMapMemory(aContext.allocator->allocator, tangentStaging.allocation, &tangentPtr); VK_SUCCESS != res)
					throw Utils::Error("Mapping memory for writing\n vmaMapMemory() returned %s", Utils::toString(res).c_str());

				std::memcpy(tangentPtr, rawData.tangents.data(), tangentSize);
				vmaUnmapMemory(aContext.allocator->allocator, tangentStaging.allocation);

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

				void* jointsPtr = nullptr;
				if (const auto res = vmaMapMemory(aContext.allocator->allocator, jointsStaging.allocation, &jointsPtr); VK_SUCCESS != res)
					throw Utils::Error("Mapping memory for writing\n vmaMapMemory() returned %s", Utils::toString(res).c_str());

				std::memcpy(jointsPtr, rawData.joints.data(), jointsSize);
				vmaUnmapMemory(aContext.allocator->allocator, jointsStaging.allocation);

				void* weightsPtr = nullptr;
				if (const auto res = vmaMapMemory(aContext.allocator->allocator, weightsStaging.allocation, &weightsPtr); VK_SUCCESS != res)
					throw Utils::Error("Mapping memory for writing\n vmaMapMemory() returned %s", Utils::toString(res).c_str());

				std::memcpy(weightsPtr, rawData.weights.data(), weightsSize);
				vmaUnmapMemory(aContext.allocator->allocator, weightsStaging.allocation);

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

				VkBufferCopy tangentCopy{};
				tangentCopy.size = tangentSize;

				vkCmdCopyBuffer(uploadCmdBuf, tangentStaging.buffer, tangentGPUBuf.buffer, 1, &tangentCopy);

				Utils::bufferBarrier(
					uploadCmdBuf,
					tangentGPUBuf.buffer,
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

				VkBufferCopy jointsCopy{};
				jointsCopy.size = jointsSize;

				vkCmdCopyBuffer(uploadCmdBuf, jointsStaging.buffer, jointsGPUBuf.buffer, 1, &jointsCopy);

				Utils::bufferBarrier(
					uploadCmdBuf,
					jointsGPUBuf.buffer,
					VK_ACCESS_TRANSFER_WRITE_BIT,
					VK_ACCESS_VERTEX_ATTRIBUTE_READ_BIT,
					VK_PIPELINE_STAGE_TRANSFER_BIT,
					VK_PIPELINE_STAGE_VERTEX_INPUT_BIT
				);

				VkBufferCopy weightsCopy{};
				weightsCopy.size = weightsSize;

				vkCmdCopyBuffer(uploadCmdBuf, weightsStaging.buffer, weightsGPUBuf.buffer, 1, &weightsCopy);

				Utils::bufferBarrier(
					uploadCmdBuf,
					weightsGPUBuf.buffer,
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

				primitive->posBuffer = std::move(posGPUBuf);
				primitive->normBuffer = std::move(normGPUBuf);
				primitive->tangentBuffer = std::move(tangentGPUBuf);
				primitive->tex0Buffer = std::move(tex0GPUBuf);
				primitive->tex1Buffer = std::move(tex1GPUBuf);
				primitive->vertColBuffer = std::move(vertColGPUBuf);
				primitive->jointsBuffer = std::move(jointsGPUBuf);
				primitive->weightsBuffer = std::move(weightsGPUBuf);
				primitive->indicesBuffer = std::move(indicesGPUBuf);

			}
		}
	}

}
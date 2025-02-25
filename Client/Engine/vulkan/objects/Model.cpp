#include "Model.hpp"

#include "../VulkanContext.hpp"
#include "../VulkanDevice.hpp"
#include "Uniforms.hpp"
#include "VulkanUtils.hpp"
#include "Error.hpp"
#include "toString.hpp"

namespace Engine {
namespace vk {

	glm::mat4 Node::getModelMatrix() {
		return glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * this->nodeMatrix;
	}

	void Model::createDescriptorSets(const VulkanContext& aContext, VkDescriptorSetLayout aSamplerSetLayout, VkDescriptorSetLayout aMaterialInfoSetLayout) {
        std::vector<glsl::MaterialInfoBuffer> materialInfos;
        
        for (Node* node : nodes) {

            if (!node->mesh) continue;

            for (Primitive* primitive : node->mesh->primitives) {
                Material& material = primitive->material;

                // Create descriptor set for sampler images
                VkDescriptorSet materialDescriptors = allocateDescriptorSet(*aContext.window, aContext.window->device->dPool, aSamplerSetLayout);
                {
                    VkWriteDescriptorSet desc[5]{};

                    VkDescriptorImageInfo baseColourInfo{};
                    baseColourInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    baseColourInfo.imageView = imageViews[material.baseColourTextureIndex].handle;
                    baseColourInfo.sampler = textures[material.baseColourTextureIndex].sampler;

                    desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    desc[0].dstSet = materialDescriptors;
                    desc[0].dstBinding = 0;
                    desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    desc[0].descriptorCount = 1;
                    desc[0].pImageInfo = &baseColourInfo;

                    VkDescriptorImageInfo metallicRoughnessInfo{};
                    metallicRoughnessInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    metallicRoughnessInfo.imageView = imageViews[material.metallicRoughnessTextureIndex].handle;
                    metallicRoughnessInfo.sampler = textures[material.metallicRoughnessTextureIndex].sampler;

                    desc[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    desc[1].dstSet = materialDescriptors;
                    desc[1].dstBinding = 1;
                    desc[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    desc[1].descriptorCount = 1;
                    desc[1].pImageInfo = &metallicRoughnessInfo;

                    VkDescriptorImageInfo emissiveInfo{};
                    emissiveInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    emissiveInfo.imageView = imageViews[material.emissiveTextureIndex].handle;
                    emissiveInfo.sampler = textures[material.emissiveTextureIndex].sampler;

                    desc[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    desc[2].dstSet = materialDescriptors;
                    desc[2].dstBinding = 2;
                    desc[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    desc[2].descriptorCount = 1;
                    desc[2].pImageInfo = &emissiveInfo;

                    VkDescriptorImageInfo occlusionInfo{};
                    occlusionInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    occlusionInfo.imageView = imageViews[material.occlusionTextureIndex].handle;
                    occlusionInfo.sampler = textures[material.occlusionTextureIndex].sampler;

                    desc[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    desc[3].dstSet = materialDescriptors;
                    desc[3].dstBinding = 3;
                    desc[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    desc[3].descriptorCount = 1;
                    desc[3].pImageInfo = &occlusionInfo;

                    VkDescriptorImageInfo normalMapInfo{};
                    normalMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    normalMapInfo.imageView = imageViews[material.normalTextureIndex].handle;
                    normalMapInfo.sampler = textures[material.normalTextureIndex].sampler;

                    desc[4].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    desc[4].dstSet = materialDescriptors;
                    desc[4].dstBinding = 4;
                    desc[4].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    desc[4].descriptorCount = 1;
                    desc[4].pImageInfo = &normalMapInfo;

                    constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
                    vkUpdateDescriptorSets(aContext.window->device->device, numSets, desc, 0, nullptr);
                }

                primitive->samplerDescriptorSet = materialDescriptors;

                // Fill vector with each material's shader dependent info
                glsl::MaterialInfoBuffer materialInfo{};
                materialInfo.alphaMode = material.alphaMode;
                materialInfo.alphaCutoff = material.alphaCutoff;
                materialInfo.emissiveFactor = glm::vec4(material.emissiveFactor, 1.0f);
                materialInfo.emissiveTexSet = material.emissiveTextureTexCoords;
                materialInfo.emissiveStrength = material.emissiveStrength;
                materialInfo.normalTexSet = material.normalTextureTexCoords;
                materialInfo.occlusionTexSet = material.occlusionTextureTexCoords;
                materialInfo.occlusionStrength = material.occlusionStrength;
                materialInfo.baseColourFactor = material.baseColourFactor;
                materialInfo.baseColourTexSet = material.baseColourTextureTexCoords;
                materialInfo.metallicRoughnessTexSet = material.metallicRoughnessTextureTexCoords;
                materialInfo.metallicFactor = material.metallicFactor;
                materialInfo.roughnessFactor = material.roughnessFactor;

                materialInfos.emplace_back(materialInfo);
            }
        }

        // Create SSBO for material infos

        // First create GPU buffer
        std::size_t materialInfoSize = materialInfos.size() * sizeof(glsl::MaterialInfoBuffer);

        // GPU sided buffer
        vk::Buffer matInfoGPUBuf = vk::createBuffer(
            *aContext.allocator,
            materialInfoSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            0,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        );

        // Staging buffer
        vk::Buffer matInfoStaging = vk::createBuffer(
            *aContext.allocator,
            materialInfoSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VMA_ALLOCATION_CREATE_HOST_ACCESS_SEQUENTIAL_WRITE_BIT
        );

        // Copy to ptr
        void* matInfoPtr = nullptr;
        if (const auto res = vmaMapMemory(aContext.allocator->allocator, matInfoStaging.allocation, &matInfoPtr); VK_SUCCESS != res)
            throw Utils::Error("Mapping memory for writing\n vmaMapMemory() returned %s", Utils::toString(res).c_str());

        std::memcpy(matInfoPtr, materialInfos.data(), materialInfoSize);
        vmaUnmapMemory(aContext.allocator->allocator, matInfoStaging.allocation);

        //vk::Fence uploadComplete = createFence(*aContext.window);

        VkCommandBuffer uploadCmdBuf = createCommandBuffer(*aContext.window);

        beginCommandBuffer(uploadCmdBuf);

        VkBufferCopy matInfoCopy{};
        matInfoCopy.size = materialInfoSize;

        vkCmdCopyBuffer(uploadCmdBuf, matInfoStaging.buffer, matInfoGPUBuf.buffer, 1, &matInfoCopy);

        endAndSubmitCommandBuffer(*aContext.window, uploadCmdBuf);

        materialInfoBuffer = std::move(matInfoGPUBuf); // Move GPU buffer into the model struct to prevent it being destroyed after scope leaves this method

        // Create descriptor set
        VkDescriptorSet materialInfoDescriptors = Engine::allocateDescriptorSet(*aContext.window, aContext.window->device->dPool, aMaterialInfoSetLayout);
        {
            VkWriteDescriptorSet desc[1]{};

            VkDescriptorBufferInfo materialUboInfo{};
            materialUboInfo.buffer = materialInfoBuffer.buffer;
            materialUboInfo.range = materialInfoSize;

            desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
            desc[0].dstSet = materialInfoDescriptors;
            desc[0].dstBinding = 0;
            desc[0].descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
            desc[0].descriptorCount = 1;
            desc[0].pBufferInfo = &materialUboInfo;

            constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
            vkUpdateDescriptorSets(aContext.window->device->device, numSets, desc, 0, nullptr);
        }

        materialInfoSSBO = materialInfoDescriptors;
	}

	void Model::drawModel(VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout) {

		VkBuffer vBuffers[5] = {
			posBuffer.buffer,
			normBuffer.buffer,
			tex0Buffer.buffer,
			tex1Buffer.buffer,
			vertColBuffer.buffer
		};
		VkBuffer iBuffer = indicesBuffer.buffer;
		VkDeviceSize vOffsets[5]{};
		VkDeviceSize iOffset{};

		vkCmdBindVertexBuffers(aCmdBuf, 0, 5, vBuffers, vOffsets);
		vkCmdBindIndexBuffer(aCmdBuf, iBuffer, 0, VK_INDEX_TYPE_UINT32);

        vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, 2, 1, &materialInfoSSBO, 0, nullptr);

		for (Node* node : nodes) {
			drawNode(node, aCmdBuf, aPipelineLayout);
		}

	}

	void Model::drawNode(Node* aNode, VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout) {
		if (aNode->mesh) {
            for (Primitive* primitive : aNode->mesh->primitives) {
                vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, 1, 1, &primitive->samplerDescriptorSet, 0, nullptr);
                
                vkCmdPushConstants(aCmdBuf, aPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, 0, sizeof(std::uint32_t), &primitive->material.index);

                vkCmdDrawIndexed(aCmdBuf, primitive->indexCount, 1, primitive->firstIndex, 0, 0);
            }
		}

		for (Node* child : aNode->children) {
			drawNode(child, aCmdBuf, aPipelineLayout);
		}
	}

	void Model::destroy() {
		// We shouldn't need to have or use this function since
		// everything in Model has its own clean up after it all
		// goes out of scope, but in the case we do need to manually
		// clean up / destroy anything, we have this function ready.
	}

}
}
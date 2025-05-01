#include "Model.hpp"

#include "Uniforms.hpp"
#include "VulkanUtils.hpp"
#include "Error.hpp"
#include "toString.hpp"
#include "../vulkan/VulkanContext.hpp"
#include "../vulkan/VulkanDevice.hpp"
#include "../vulkan/PipelineCreation.hpp"
#include "../vulkan/Renderer.hpp"

namespace Engine {
namespace vk {

    vk::Node* Model::getNodeFromIndex(int nodeIndex) {
        for (vk::Node* node : linearNodes) {
            if (node->index == nodeIndex)
                return node;
        }

        return nullptr;
    }

	void Model::createDescriptorSets(
        const VulkanContext& aContext, 
        VkDescriptorSetLayout aSamplerSetLayout, 
        VkDescriptorSetLayout aMaterialInfoSetLayout) 
    {
        std::vector<glsl::MaterialInfoBuffer> materialInfos;
        
        for (Node* node : linearNodes) {

            if (!node->mesh) continue;

            for (Primitive* primitive : node->mesh->primitives) {
                Material& material = primitive->material;

                // Create descriptor set for sampler images
                VkDescriptorSet materialDescriptors = allocateDescriptorSet(*aContext.window, aContext.window->device->dPool, aSamplerSetLayout);
                {
                    VkWriteDescriptorSet desc[5]{};

                    VkImageView baseColourImageView = material.baseColourTextureIndex != -1 ? imageViews[material.baseColourTextureIndex].handle : dummyImageView.handle;
                    VkSampler baseColourSampler = material.baseColourTextureIndex != -1 ? textures[material.baseColourTextureIndex].sampler : defaultSampler.handle;
                    VkDescriptorImageInfo baseColourInfo{};
                    baseColourInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    baseColourInfo.imageView = baseColourImageView;
                    baseColourInfo.sampler = baseColourSampler;

                    desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    desc[0].dstSet = materialDescriptors;
                    desc[0].dstBinding = 0;
                    desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    desc[0].descriptorCount = 1;
                    desc[0].pImageInfo = &baseColourInfo;

                    VkImageView metallicRoughnessImageView = material.metallicRoughnessTextureIndex != -1 ? imageViews[material.metallicRoughnessTextureIndex].handle : dummyImageView.handle;
                    VkSampler metallicRoughnessSampler = material.metallicRoughnessTextureIndex != -1 ? textures[material.metallicRoughnessTextureIndex].sampler : defaultSampler.handle;
                    VkDescriptorImageInfo metallicRoughnessInfo{};
                    metallicRoughnessInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    metallicRoughnessInfo.imageView = metallicRoughnessImageView;
                    metallicRoughnessInfo.sampler = metallicRoughnessSampler;

                    desc[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    desc[1].dstSet = materialDescriptors;
                    desc[1].dstBinding = 1;
                    desc[1].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    desc[1].descriptorCount = 1;
                    desc[1].pImageInfo = &metallicRoughnessInfo;

                    VkImageView emissiveImageView = material.emissiveTextureIndex != -1 ? imageViews[material.emissiveTextureIndex].handle : dummyImageView.handle;
                    VkSampler emissiveSampler = material.emissiveTextureIndex != -1 ? textures[material.emissiveTextureIndex].sampler : defaultSampler.handle;
                    VkDescriptorImageInfo emissiveInfo{};
                    emissiveInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    emissiveInfo.imageView = emissiveImageView;
                    emissiveInfo.sampler = emissiveSampler;

                    desc[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    desc[2].dstSet = materialDescriptors;
                    desc[2].dstBinding = 2;
                    desc[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    desc[2].descriptorCount = 1;
                    desc[2].pImageInfo = &emissiveInfo;

                    VkImageView occlusionImageView = material.occlusionTextureIndex != -1 ? imageViews[material.occlusionTextureIndex].handle : dummyImageView.handle;
                    VkSampler occlusionSampler = material.occlusionTextureIndex != -1 ? textures[material.occlusionTextureIndex].sampler : defaultSampler.handle;
                    VkDescriptorImageInfo occlusionInfo{};
                    occlusionInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    occlusionInfo.imageView = occlusionImageView;
                    occlusionInfo.sampler = occlusionSampler;

                    desc[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
                    desc[3].dstSet = materialDescriptors;
                    desc[3].dstBinding = 3;
                    desc[3].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
                    desc[3].descriptorCount = 1;
                    desc[3].pImageInfo = &occlusionInfo;

                    VkImageView normalImageView = material.normalTextureIndex != -1 ? imageViews[material.normalTextureIndex].handle : dummyImageView.handle;
                    VkSampler normalSampler = material.normalTextureIndex != -1 ? textures[material.normalTextureIndex].sampler : defaultSampler.handle;
                    VkDescriptorImageInfo normalMapInfo{};
                    normalMapInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
                    normalMapInfo.imageView = normalImageView;
                    normalMapInfo.sampler = normalSampler;

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
            "matInfoGPU",
            *aContext.allocator,
            materialInfoSize,
            VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            0,
            VMA_MEMORY_USAGE_AUTO_PREFER_DEVICE
        );

        // Staging buffer
        vk::Buffer matInfoStaging = vk::createBuffer(
            "matInfoStaging",
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

	void Model::drawModel(VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout, bool justGeometry) {
        if (justGeometry) {

            // Draw opaque nodes first
            for (Node* node : linearNodes) {
                vkCmdPushConstants(aCmdBuf, aPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &node->getGlobalMatrix()[0]); // Model matrix
                drawNodeGeometry(node, aCmdBuf, aPipelineLayout, AlphaMode::ALPHA_OPAQUE);
            }

            // Do we want to do alpha masking on the shadow pass?
            // For some models it may make sense if there are large
            // parts of a mesh that need to be alpha masked and end
            // subsequently end up being transparent, but will still
            // cause a shadow.
            // Alpha masking the shadow pass would mean passing in
            // the base colour texture and material info SSBO.
         
            // Draw alpha masked nodes second
            //for (Node* node : nodes) {
            //    vkCmdPushConstants(aCmdBuf, aPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &node->getGlobalMatrix()[0]);
            //    drawNodeGeometry(node, aCmdBuf, AlphaMode::ALPHA_MASK);
            //}

            return;
        }

        vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, 3, 1, &materialInfoSSBO, 0, nullptr); // Material SSBO

        vkCmdSetCullMode(aCmdBuf, VK_CULL_MODE_BACK_BIT);

        // Draw opaque nodes first
		for (Node* node : linearNodes) {
            vkCmdPushConstants(aCmdBuf, aPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &node->getGlobalMatrix()[0]);
			drawNode(node, aCmdBuf, aPipelineLayout, AlphaMode::ALPHA_OPAQUE);
		}

        vkCmdSetCullMode(aCmdBuf, VK_CULL_MODE_NONE);

        // Draw alpha masked nodes second
        for (Node* node : linearNodes) {
            vkCmdPushConstants(aCmdBuf, aPipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, sizeof(glm::mat4), &node->getGlobalMatrix()[0]);
            drawNode(node, aCmdBuf, aPipelineLayout, AlphaMode::ALPHA_MASK);
        }
	}

	void Model::drawNode(Node* aNode, VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout, AlphaMode aAlphaMode) {
		if (aNode->mesh) {
            for (std::size_t i = 0; i < aNode->mesh->primitives.size(); i++) {
                Primitive* primitive = aNode->mesh->primitives[i];

                if (primitive->material.alphaMode != aAlphaMode)
                    continue;

                vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, 1, 1, &aNode->descriptor, 0, nullptr); // Joint matrices
                vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, 2, 1, &primitive->samplerDescriptorSet, 0, nullptr); // Textures
                vkCmdPushConstants(aCmdBuf, aPipelineLayout, VK_SHADER_STAGE_FRAGMENT_BIT, sizeof(glm::mat4), sizeof(std::uint32_t), &i); // Material index

                VkBuffer vBuffers[8] = {
                    primitive->posBuffer.buffer,
                    primitive->normBuffer.buffer,
                    primitive->tangentBuffer.buffer,
                    primitive->tex0Buffer.buffer,
                    primitive->tex1Buffer.buffer,
                    primitive->vertColBuffer.buffer,
                    primitive->jointsBuffer.buffer,
                    primitive->weightsBuffer.buffer
                };
                VkBuffer iBuffer = primitive->indicesBuffer.buffer;
                VkDeviceSize vOffsets[8]{};
                VkDeviceSize iOffset{};

                vkCmdBindVertexBuffers(aCmdBuf, 0, 8, vBuffers, vOffsets);
                vkCmdBindIndexBuffer(aCmdBuf, iBuffer, 0, VK_INDEX_TYPE_UINT32);

                vkCmdDrawIndexed(aCmdBuf, primitive->indexCount, 1, primitive->firstIndex, 0, 0);
            }
		}
	}

    void Model::drawNodeGeometry(Node* aNode, VkCommandBuffer aCmdBuf, VkPipelineLayout aPipelineLayout, AlphaMode aAlphaMode) {
        if (aNode->mesh) {
            for (Primitive* primitive : aNode->mesh->primitives) {

                vkCmdBindDescriptorSets(aCmdBuf, VK_PIPELINE_BIND_POINT_GRAPHICS, aPipelineLayout, 1, 1, &aNode->descriptor, 0, nullptr); // Joint matrices

                VkBuffer vBuffers[3] = {
                    primitive->posBuffer.buffer,
                    primitive->jointsBuffer.buffer,
                    primitive->weightsBuffer.buffer
                };
                VkBuffer iBuffer = primitive->indicesBuffer.buffer;
                VkDeviceSize vOffsets[3]{};
                VkDeviceSize iOffset{};

                vkCmdBindVertexBuffers(aCmdBuf, 0, 3, vBuffers, vOffsets);
                vkCmdBindIndexBuffer(aCmdBuf, iBuffer, 0, VK_INDEX_TYPE_UINT32);

                vkCmdDrawIndexed(aCmdBuf, primitive->indexCount, 1, primitive->firstIndex, 0, 0);
            }
        }
    }

    void Model::playAnimation() {
        if (this->animationQueue.size() == 0) {
            this->idleAnimation.restart();
            this->animationQueue.push(this->animations[this->animationIndex]);
            this->blending = true;
        }
    }

    void Model::updateAnimation(float timeDelta) {
        // Animation queue is empty: 
        // 1. Render idle animation
        // Animation queue is not empty:
        // 1. Blend to first animation in queue
        // 2. If blended, continue running first animation in queue
        // 3. Once first animation is completed, pop animation
        // 4. Check if animation queue is empty. TODO
        //    - If so, blend to idle and run idle.
        //    - If not, go to step 1.


        // If animation queue is empty
        if (this->animationQueue.empty()) {
            // and blending is enabled then blend to idle animation
            if (this->blending) {
                this->blendAnimation(this->idleAnimation, timeDelta, 0.5f);
            }
            // Otherwise just update idle animation
            else {
                this->idleAnimation.update(*this, timeDelta);
            }
        }
        // Process animation queue
        else {
            // Get first animation in queue
            vk::Animation& target = this->animationQueue.front();

            // If this model is still blending, run blendAnimation
            if (this->blending) {
                this->blendAnimation(target, timeDelta, 0.2f);
            }
            // If not blending then just continue updating front animation
            else {
                target.update(*this, timeDelta);
            }

            // Check if target animation is finished
            if (!target.animating) {
                // If finished, pop animation from queue and initiate blending to next animation
                this->animationQueue.pop();
                this->blending = true;
            }
        }
    }

    // This method is actually extremely similar to Animation::update() and so could be merged in some way
    void Model::blendAnimation(Animation& target, float timeDelta, float interpolationTime) {
        // Kick off target animation if not already
        if (!target.animating)
            target.animating = true;

        bool updated = false;

        float interp = this->blendingTimer / interpolationTime;

        // It interp is greater than 1, we have blended into the target animation
        if (interp > 1.0f) {
            this->blending = false;
            this->blendingTimer = 0.0f;
            target.timer += timeDelta;
            return;
        }

        for (AnimationChannel& channel : target.channels) {
            vk::AnimationSampler& sampler = target.samplers[channel.samplerIndex];

            for (std::size_t i = 0; i < sampler.keyframeTimes.size() - 1; i++) {
                float firstKeyframe = sampler.keyframeTimes[i];
                float secondKeyframe = sampler.keyframeTimes[i + 1];

                if ((target.timer < firstKeyframe) || (target.timer > secondKeyframe))
                    continue;

                switch (channel.pathType) {
                case vk::AnimationChannel::PathType::TRANSLATION:
                    sampler.translate(channel.node, interp, i, true);
                    break;
                case vk::AnimationChannel::PathType::ROTATION:
                    sampler.rotate(channel.node, interp, i, true);
                    break;
                case vk::AnimationChannel::PathType::SCALE:
                    sampler.scale(channel.node, interp, i, true);
                    break;
                }

                updated = true;
            }
        }

        if (updated) {
            // Update joint matrices of model
            for (Node* node : this->linearNodes)
                node->update();
        }

        this->blendingTimer += timeDelta;
        target.timer += timeDelta;
    }

	void Model::destroy() {
        for (Node* node : linearNodes) {

            if (!node->mesh) continue;

            for (Primitive* primitive : node->mesh->primitives)
                primitive->~Primitive();

            node->descriptorBuffer.~Buffer();
        }
	}

}
}
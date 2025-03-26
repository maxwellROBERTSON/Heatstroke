#include "PipelineCreation.hpp"

#include <cassert>

#include "Error.hpp"
#include "toString.hpp"
#include "ShaderPaths.hpp"
#include "VulkanContext.hpp"
#include "VulkanDevice.hpp"
#include "Utils.hpp"
#include "vulkan/vulkan_core.h"

namespace Engine {

	vk::ShaderModule loadShaderModule(const VulkanWindow& aWindow, const char* aSpirvPath) {
		assert(aSpirvPath);

		if (std::FILE* fin = std::fopen(aSpirvPath, "rb")) {
			std::fseek(fin, 0, SEEK_END);
			const auto bytes = std::size_t(std::ftell(fin));
			std::fseek(fin, 0, SEEK_SET);

			assert(0 == bytes % 4);
			const auto words = bytes / 4;

			std::vector<std::uint32_t> code(words);

			std::size_t offset = 0;
			while (offset != words) {
				const auto read = std::fread(code.data() + offset, sizeof(std::uint32_t), words - offset, fin);

				if (0 == read) {
					const auto err = std::ferror(fin), eof = std::feof(fin);
					std::fclose(fin);

					throw Utils::Error("Error reading '%s': ferror: %d, feof = %d", aSpirvPath, err, eof);
				}

				offset += read;
			}

			std::fclose(fin);

			VkShaderModuleCreateInfo moduleInfo{};
			moduleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
			moduleInfo.codeSize = bytes;
			moduleInfo.pCode = code.data();

			VkShaderModule smod = VK_NULL_HANDLE;
			if (const auto res = vkCreateShaderModule(aWindow.device->device, &moduleInfo, nullptr, &smod); VK_SUCCESS != res) {
				throw Utils::Error("Unable to create shader module from %s\n vkShaderCreateShaderModule() returned %s", aSpirvPath, Utils::toString(res).c_str());
			}

			return vk::ShaderModule(aWindow.device->device, smod);
		}

		throw Utils::Error("Cannot open '%s' for reading", aSpirvPath);
	}

	vk::RenderPass createRenderPass(const VulkanWindow& aWindow) {
		VkAttachmentDescription attachments[2]{};
		attachments[0].format = aWindow.swapchainFormat;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		attachments[1].format = VK_FORMAT_D32_SFLOAT_S8_UINT; // Maybe dont need 32-bit for depth. Maybe dont even need this exact format if we dont end up doing anything with the stencil buffer
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[1].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

		VkAttachmentReference subpassAttachments[1]{};
		subpassAttachments[0].attachment = 0;
		subpassAttachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachment{};
		depthAttachment.attachment = 1;
		depthAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpasses[1]{};
		subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpasses[0].colorAttachmentCount = 1;
		subpasses[0].pColorAttachments = subpassAttachments;
		subpasses[0].pDepthStencilAttachment = &depthAttachment;

		VkSubpassDependency deps[2]{};
		deps[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		deps[0].srcAccessMask = 0;
		deps[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		deps[0].dstSubpass = 0;
		deps[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		deps[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		deps[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		deps[1].srcSubpass = VK_SUBPASS_EXTERNAL;
		deps[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		deps[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		deps[1].dstSubpass = 0;
		deps[1].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		deps[1].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

		VkRenderPassCreateInfo passInfo{};
		passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		passInfo.attachmentCount = 2;
		passInfo.pAttachments = attachments;
		passInfo.subpassCount = 1;
		passInfo.pSubpasses = subpasses;
		passInfo.dependencyCount = 2;
		passInfo.pDependencies = deps;

		VkRenderPass rpass = VK_NULL_HANDLE;
		if (const auto res = vkCreateRenderPass(aWindow.device->device, &passInfo, nullptr, &rpass); VK_SUCCESS != res) {
			throw Utils::Error("Unable to create render pass\n vkCreateRenderPass() returned %s\n", Utils::toString(res).c_str());
		}

		return vk::RenderPass(aWindow.device->device, rpass);
	}

	vk::RenderPass createDeferredRenderPass(const VulkanWindow& aWindow) {
		// 2 Passes:
		// Pass 1: Geometry pass writes data to G-buffers
		// Pass 2: Does shading for lighting

		VkAttachmentDescription attachments[5]{};
		// Swap chain image attachment
		attachments[0].format = aWindow.swapchainFormat;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

		// Normal Attachment
		attachments[1].format = VK_FORMAT_R16G16B16A16_SFLOAT;
		attachments[1].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[1].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[1].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		// Albedo Attachment
		attachments[2].format = VK_FORMAT_R16G16B16A16_SFLOAT;
		attachments[2].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[2].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		// Emissive Attachment
		attachments[3].format = VK_FORMAT_R16G16B16A16_SFLOAT;
		attachments[3].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[3].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[3].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
		attachments[3].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[3].finalLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		// Depth Attachment
		attachments[4].format = VK_FORMAT_D32_SFLOAT_S8_UINT;
		attachments[4].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[4].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[4].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[4].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[4].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
		attachments[4].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[4].stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;

		VkAttachmentReference swapchainAttachment{};
		swapchainAttachment.attachment = 0;
		swapchainAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachment{};
		depthAttachment.attachment = 4;
		depthAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkAttachmentReference renderTargetsAttachments[3]{};
		renderTargetsAttachments[0].attachment = 1;
		renderTargetsAttachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		renderTargetsAttachments[1].attachment = 2;
		renderTargetsAttachments[1].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
		renderTargetsAttachments[2].attachment = 3;
		renderTargetsAttachments[2].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpasses[2]{};
		subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpasses[0].colorAttachmentCount = 3;
		subpasses[0].pColorAttachments = renderTargetsAttachments;
		subpasses[0].pDepthStencilAttachment = &depthAttachment;

		subpasses[1].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpasses[1].colorAttachmentCount = 1;
		subpasses[1].pColorAttachments = &swapchainAttachment;

		VkAttachmentReference inputAttachments[4]{};
		inputAttachments[0].attachment = 1;
		inputAttachments[0].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		inputAttachments[1].attachment = 2;
		inputAttachments[1].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		inputAttachments[2].attachment = 3;
		inputAttachments[2].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		inputAttachments[3].attachment = 4;
		inputAttachments[3].layout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;

		subpasses[1].inputAttachmentCount = 4;
		subpasses[1].pInputAttachments = inputAttachments;

		VkSubpassDependency deps[4]{};
		deps[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		deps[0].srcAccessMask = 0;
		deps[0].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		deps[0].dstSubpass = 0;
		deps[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		deps[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

		deps[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		deps[1].srcSubpass = VK_SUBPASS_EXTERNAL;
		deps[1].srcAccessMask = 0;
		deps[1].srcStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;
		deps[1].dstSubpass = 1;
		deps[1].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		deps[1].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

		deps[2].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		deps[2].srcSubpass = 0;
		deps[2].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT;
		deps[2].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		deps[2].dstSubpass = 1;
		deps[2].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT | VK_ACCESS_INPUT_ATTACHMENT_READ_BIT;
		deps[2].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;

		deps[3].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		deps[3].srcSubpass = 1;
		deps[3].srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		deps[3].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT | VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		deps[3].dstSubpass = VK_SUBPASS_EXTERNAL;
		deps[3].dstAccessMask = 0;
		deps[3].dstStageMask = VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT;

		VkRenderPassCreateInfo passInfo{};
		passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		passInfo.attachmentCount = 5;
		passInfo.pAttachments = attachments;
		passInfo.subpassCount = 2;
		passInfo.pSubpasses = subpasses;
		passInfo.dependencyCount = 4;
		passInfo.pDependencies = deps;

		VkRenderPass rpass = VK_NULL_HANDLE;
		if (const auto res = vkCreateRenderPass(aWindow.device->device, &passInfo, nullptr, &rpass); VK_SUCCESS != res) {
			throw Utils::Error("Unable to create render pass\n vkCreateRenderPass() returned %s\n", Utils::toString(res).c_str());
		}

		return vk::RenderPass(aWindow.device->device, rpass);
	}

	vk::RenderPass createShadowRenderPass(const VulkanWindow& aWindow) {
		VkAttachmentDescription attachments[1]{};
		attachments[0].format = VK_FORMAT_D32_SFLOAT_S8_UINT;
		attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
		attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
		attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
		attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
		attachments[0].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
		attachments[0].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

		VkAttachmentReference depthAttachment{};
		depthAttachment.attachment = 0;
		depthAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpasses[1]{};
		subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpasses[0].colorAttachmentCount = 0;
		subpasses[0].pDepthStencilAttachment = &depthAttachment;

		VkSubpassDependency deps[2]{};
		deps[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
		deps[0].srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		deps[0].srcStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
		deps[0].dstSubpass = 0;
		deps[0].dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		deps[0].dstStageMask = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;

		deps[1].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
		deps[1].srcSubpass = 0;
		deps[1].srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		deps[1].srcStageMask = VK_PIPELINE_STAGE_LATE_FRAGMENT_TESTS_BIT;
		deps[1].dstSubpass = VK_SUBPASS_EXTERNAL;
		deps[1].dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		deps[1].dstStageMask = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;

		VkRenderPassCreateInfo passInfo{};
		passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		passInfo.attachmentCount = 1;
		passInfo.pAttachments = attachments;
		passInfo.subpassCount = 1;
		passInfo.pSubpasses = subpasses;
		passInfo.dependencyCount = 2;
		passInfo.pDependencies = deps;

		VkRenderPass rpass = VK_NULL_HANDLE;
		if (const auto res = vkCreateRenderPass(aWindow.device->device, &passInfo, nullptr, &rpass); VK_SUCCESS != res) {
			throw Utils::Error("Unable to create render pass\n vkCreateRenderPass() returned %s\n", Utils::toString(res).c_str());
		}

		return vk::RenderPass(aWindow.device->device, rpass);
	}

	vk::DescriptorSetLayout createDescriptorLayout(const VulkanWindow& aWindow, std::vector<DescriptorSetting> aDescriptorSettings) {
		std::vector<VkDescriptorSetLayoutBinding> layoutBindings;
		
		for (std::size_t i = 0; i < aDescriptorSettings.size(); i++) {
			VkDescriptorSetLayoutBinding binding{};
			binding.binding = i;
			binding.descriptorType = aDescriptorSettings[i].descriptorType;
			binding.descriptorCount = 1;
			binding.stageFlags = aDescriptorSettings[i].shaderStageFlags;

			layoutBindings.emplace_back(binding);
		}

		VkDescriptorSetLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = layoutBindings.size();
		layoutInfo.pBindings = layoutBindings.data();

		VkDescriptorSetLayout layout = VK_NULL_HANDLE;
		if (const auto res = vkCreateDescriptorSetLayout(aWindow.device->device, &layoutInfo, nullptr, &layout); VK_SUCCESS != res)
			throw Utils::Error("Unable to create descriptor set layout\n vkCreateDescriptorSetLayout() returned %s", Utils::toString(res).c_str());

		return vk::DescriptorSetLayout(aWindow.device->device, layout);
	}

	vk::PipelineLayout createPipelineLayout(const VulkanWindow& aWindow, std::vector<VkDescriptorSetLayout>& aDescriptorSetLayouts, bool aNeedPushConstant) {
		VkPushConstantRange pushConstantRange{};
		
		if (aNeedPushConstant) {
			// This always assumes our push constant will be 1 integer in the fragment shader. Until
			// we need a different push constant this should be fine for now.
			pushConstantRange.size = sizeof(std::uint32_t);
			pushConstantRange.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
		}

		VkPipelineLayoutCreateInfo layoutInfo{};
		layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		layoutInfo.setLayoutCount = aDescriptorSetLayouts.size();
		layoutInfo.pSetLayouts = aDescriptorSetLayouts.data();
		layoutInfo.pushConstantRangeCount = aNeedPushConstant ? 1 : 0;
		layoutInfo.pPushConstantRanges = &pushConstantRange;

		VkPipelineLayout layout = VK_NULL_HANDLE;
		if (const auto res = vkCreatePipelineLayout(aWindow.device->device, &layoutInfo, nullptr, &layout); VK_SUCCESS != res) {
			throw Utils::Error("Unable to create pipeline layout\n vkCreatePipelineLayout() returned %s", Utils::toString(res).c_str());
		}

		return vk::PipelineLayout(aWindow.device->device, layout);
	}

	vk::Pipeline createForwardPipeline(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aPipelineLayout, bool shadows) {
		vk::ShaderModule vert = loadShaderModule(aWindow, Shaders::forwardVert);
		vk::ShaderModule frag = loadShaderModule(aWindow, Shaders::forwardFrag);

		if (shadows) {
			vert = loadShaderModule(aWindow, Shaders::forwardShadowVert);
			frag = loadShaderModule(aWindow, Shaders::forwardShadowFrag);
		}

		VkPipelineShaderStageCreateInfo stages[2]{};
		stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		stages[0].module = vert.handle;
		stages[0].pName = "main";

		stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stages[1].module = frag.handle;
		stages[1].pName = "main";

		// Vertex attributes:
		// 1. Positions
		// 2. Normals
		// 3. Tangents
		// 4. Texture coordinates 0
		// 5. Texture coordinates 1 (in glTF some textures refer to a different set of texture coordinates
		//	  so we have to account for multiple sets of texture coordinates. In theory any number
		//	  of texture coordinates could be defined, but in practice with our current models only
		//    one other texture coordinate set is referred to)
		// 6. Vertex colours (vertex colours is a valid glTF vertex attribute but will most likely not exist
		//    at the same time as texture coordinates, so a optimisation would be to dynamically create render passes
		//    based on only the attributes that exist in the glTF file, so that we are not sending both at the same time
		//    as one will just be default values)
		VkVertexInputBindingDescription vertexInputs[6]{};
		vertexInputs[0].binding = 0;
		vertexInputs[0].stride = sizeof(float) * 3;
		vertexInputs[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputs[1].binding = 1;
		vertexInputs[1].stride = sizeof(float) * 3;
		vertexInputs[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputs[2].binding = 2;
		vertexInputs[2].stride = sizeof(float) * 4;
		vertexInputs[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputs[3].binding = 3;
		vertexInputs[3].stride = sizeof(float) * 2;
		vertexInputs[3].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputs[4].binding = 4;
		vertexInputs[4].stride = sizeof(float) * 2;
		vertexInputs[4].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputs[5].binding = 5;
		vertexInputs[5].stride = sizeof(float) * 4;
		vertexInputs[5].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputAttributeDescription vertexAttributes[6]{};
		vertexAttributes[0].binding = 0;
		vertexAttributes[0].location = 0;
		vertexAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexAttributes[0].offset = 0;
		vertexAttributes[1].binding = 1;
		vertexAttributes[1].location = 1;
		vertexAttributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexAttributes[1].offset = 0;
		vertexAttributes[2].binding = 2;
		vertexAttributes[2].location = 2;
		vertexAttributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexAttributes[2].offset = 0;
		vertexAttributes[3].binding = 3;
		vertexAttributes[3].location = 3;
		vertexAttributes[3].format = VK_FORMAT_R32G32_SFLOAT;
		vertexAttributes[3].offset = 0;
		vertexAttributes[4].binding = 4;
		vertexAttributes[4].location = 4;
		vertexAttributes[4].format = VK_FORMAT_R32G32_SFLOAT;
		vertexAttributes[4].offset = 0;
		vertexAttributes[5].binding = 5;
		vertexAttributes[5].location = 5;
		vertexAttributes[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexAttributes[5].offset = 0;

		VkPipelineVertexInputStateCreateInfo inputInfo{};
		inputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		inputInfo.vertexBindingDescriptionCount = 6;
		inputInfo.pVertexBindingDescriptions = vertexInputs;
		inputInfo.vertexAttributeDescriptionCount = 6;
		inputInfo.pVertexAttributeDescriptions = vertexAttributes;

		VkPipelineInputAssemblyStateCreateInfo assemblyInfo{};
		assemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		assemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = float(aWindow.swapchainExtent.width);
		viewport.height = float(aWindow.swapchainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = VkOffset2D{ 0, 0 };
		scissor.extent = aWindow.swapchainExtent;

		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterInfo{};
		rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterInfo.depthClampEnable = VK_FALSE;
		rasterInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;
		rasterInfo.depthBiasEnable = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterInfo.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo samplingInfo{};
		samplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		samplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState blendStates[1]{};
		blendStates[0].blendEnable = VK_FALSE;
		blendStates[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo blendInfo{};
		blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		blendInfo.logicOpEnable = VK_FALSE;
		blendInfo.attachmentCount = 1;
		blendInfo.pAttachments = blendStates;

		VkPipelineDepthStencilStateCreateInfo depthInfo{};
		depthInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthInfo.depthTestEnable = VK_TRUE;
		depthInfo.depthWriteEnable = VK_TRUE;
		depthInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthInfo.minDepthBounds = 0.0f;
		depthInfo.maxDepthBounds = 1.0f;

		VkGraphicsPipelineCreateInfo pipeInfo{};
		pipeInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeInfo.stageCount = 2;
		pipeInfo.pStages = stages;
		pipeInfo.pVertexInputState = &inputInfo;
		pipeInfo.pInputAssemblyState = &assemblyInfo;
		pipeInfo.pTessellationState = nullptr;
		pipeInfo.pViewportState = &viewportInfo;
		pipeInfo.pRasterizationState = &rasterInfo;
		pipeInfo.pMultisampleState = &samplingInfo;
		pipeInfo.pDepthStencilState = &depthInfo;
		pipeInfo.pColorBlendState = &blendInfo;
		pipeInfo.pDynamicState = nullptr;
		pipeInfo.layout = aPipelineLayout;
		pipeInfo.renderPass = aRenderPass;
		pipeInfo.subpass = 0;

		VkPipeline pipe = VK_NULL_HANDLE;
		if (const auto res = vkCreateGraphicsPipelines(aWindow.device->device, VK_NULL_HANDLE, 1, &pipeInfo, nullptr, &pipe); VK_SUCCESS != res) {
			throw Utils::Error("Unable to create graphics pipeline\n vkCreateGraphicsPipeline() returned %s", Utils::toString(res).c_str());
		}

		return vk::Pipeline(aWindow.device->device, pipe);
	}

	std::tuple<vk::Pipeline, vk::Pipeline> createDeferredPipelines(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aGBufWriteLayout, VkPipelineLayout aShadingLayout) {
		vk::ShaderModule gBufVert = loadShaderModule(aWindow, Shaders::gBufWriteVert);
		vk::ShaderModule gBufFrag = loadShaderModule(aWindow, Shaders::gBufWriteFrag);

		VkPipelineShaderStageCreateInfo stages[2]{};
		stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		stages[0].module = gBufVert.handle;
		stages[0].pName = "main";

		stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stages[1].module = gBufFrag.handle;
		stages[1].pName = "main";

		// Vertex attributes:
		// 1. Positions
		// 2. Normals
		// 3. Tangents
		// 4. Texture coordinates 0
		// 5. Texture coordinates 1
		// 6. Vertex colours
		VkVertexInputBindingDescription vertexInputs[6]{};
		vertexInputs[0].binding = 0;
		vertexInputs[0].stride = sizeof(float) * 3;
		vertexInputs[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputs[1].binding = 1;
		vertexInputs[1].stride = sizeof(float) * 3;
		vertexInputs[1].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputs[2].binding = 2;
		vertexInputs[2].stride = sizeof(float) * 4;
		vertexInputs[2].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputs[3].binding = 3;
		vertexInputs[3].stride = sizeof(float) * 2;
		vertexInputs[3].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputs[4].binding = 4;
		vertexInputs[4].stride = sizeof(float) * 2;
		vertexInputs[4].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		vertexInputs[5].binding = 5;
		vertexInputs[5].stride = sizeof(float) * 4;
		vertexInputs[5].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputAttributeDescription vertexAttributes[6]{};
		vertexAttributes[0].binding = 0;
		vertexAttributes[0].location = 0;
		vertexAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexAttributes[0].offset = 0;
		vertexAttributes[1].binding = 1;
		vertexAttributes[1].location = 1;
		vertexAttributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexAttributes[1].offset = 0;
		vertexAttributes[2].binding = 2;
		vertexAttributes[2].location = 2;
		vertexAttributes[2].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexAttributes[2].offset = 0;
		vertexAttributes[3].binding = 3;
		vertexAttributes[3].location = 3;
		vertexAttributes[3].format = VK_FORMAT_R32G32_SFLOAT;
		vertexAttributes[3].offset = 0;
		vertexAttributes[4].binding = 4;
		vertexAttributes[4].location = 4;
		vertexAttributes[4].format = VK_FORMAT_R32G32_SFLOAT;
		vertexAttributes[4].offset = 0;
		vertexAttributes[5].binding = 5;
		vertexAttributes[5].location = 5;
		vertexAttributes[5].format = VK_FORMAT_R32G32B32A32_SFLOAT;
		vertexAttributes[5].offset = 0;

		VkPipelineVertexInputStateCreateInfo inputInfo{};
		inputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		inputInfo.vertexBindingDescriptionCount = 6;
		inputInfo.pVertexBindingDescriptions = vertexInputs;
		inputInfo.vertexAttributeDescriptionCount = 6;
		inputInfo.pVertexAttributeDescriptions = vertexAttributes;

		VkPipelineInputAssemblyStateCreateInfo assemblyInfo{};
		assemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		assemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = float(aWindow.swapchainExtent.width);
		viewport.height = float(aWindow.swapchainExtent.height);
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = VkOffset2D{ 0, 0 };
		scissor.extent = aWindow.swapchainExtent;

		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterInfo{};
		rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterInfo.depthClampEnable = VK_FALSE;
		rasterInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterInfo.cullMode = VK_CULL_MODE_BACK_BIT;	
		rasterInfo.depthBiasEnable = VK_FRONT_FACE_COUNTER_CLOCKWISE;
		rasterInfo.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo samplingInfo{};
		samplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		samplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState blendStates[3]{};
		blendStates[0].blendEnable = VK_FALSE;
		blendStates[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendStates[1].blendEnable = VK_FALSE;
		blendStates[1].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
		blendStates[2].blendEnable = VK_FALSE;
		blendStates[2].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo blendInfo{};
		blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		blendInfo.logicOpEnable = VK_FALSE;
		blendInfo.attachmentCount = 3;
		blendInfo.pAttachments = blendStates;

		VkPipelineDepthStencilStateCreateInfo depthInfo{};
		depthInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthInfo.depthTestEnable = VK_TRUE;
		depthInfo.depthWriteEnable = VK_TRUE;
		depthInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthInfo.minDepthBounds = 0.0f;
		depthInfo.maxDepthBounds = 1.0f;

		VkGraphicsPipelineCreateInfo pipeInfo{};
		pipeInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeInfo.stageCount = 2;
		pipeInfo.pStages = stages;
		pipeInfo.pVertexInputState = &inputInfo;
		pipeInfo.pInputAssemblyState = &assemblyInfo;
		pipeInfo.pTessellationState = nullptr;
		pipeInfo.pViewportState = &viewportInfo;
		pipeInfo.pRasterizationState = &rasterInfo;
		pipeInfo.pMultisampleState = &samplingInfo;
		pipeInfo.pDepthStencilState = &depthInfo;
		pipeInfo.pColorBlendState = &blendInfo;
		pipeInfo.pDynamicState = nullptr;
		pipeInfo.layout = aGBufWriteLayout;
		pipeInfo.renderPass = aRenderPass;
		pipeInfo.subpass = 0;

		VkPipeline gBufWritePipe = VK_NULL_HANDLE;
		if (const auto res = vkCreateGraphicsPipelines(aWindow.device->device, VK_NULL_HANDLE, 1, &pipeInfo, nullptr, &gBufWritePipe); VK_SUCCESS != res) {
			throw Utils::Error("Unable to create graphics pipeline\n vkCreateGraphicsPipeline() returned %s", Utils::toString(res).c_str());
		}

		vk::ShaderModule deferredVert = loadShaderModule(aWindow, Shaders::deferredVert);
		vk::ShaderModule deferredFrag = loadShaderModule(aWindow, Shaders::deferredFrag);

		stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		stages[0].module = deferredVert.handle;
		stages[0].pName = "main";

		stages[1].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[1].stage = VK_SHADER_STAGE_FRAGMENT_BIT;
		stages[1].module = deferredFrag.handle;
		stages[1].pName = "main";

		VkPipelineVertexInputStateCreateInfo emptyVertexState{};
		emptyVertexState.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

		VkPipelineColorBlendAttachmentState blendStates2[1]{};
		blendStates2[0].blendEnable = VK_FALSE;
		blendStates2[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo blendInfo2{};
		blendInfo2.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		blendInfo2.logicOpEnable = VK_FALSE;
		blendInfo2.attachmentCount = 1;
		blendInfo2.pAttachments = blendStates2;

		rasterInfo.cullMode = VK_CULL_MODE_NONE;

		pipeInfo.pVertexInputState = &emptyVertexState;
		pipeInfo.pColorBlendState = &blendInfo2;
		pipeInfo.layout = aShadingLayout;
		pipeInfo.subpass = 1;

		VkPipeline shadingPipe = VK_NULL_HANDLE;
		if (const auto res = vkCreateGraphicsPipelines(aWindow.device->device, VK_NULL_HANDLE, 1, &pipeInfo, nullptr, &shadingPipe); VK_SUCCESS != res) {
			throw Utils::Error("Unable to create graphics pipeline\n vkCreateGraphicsPipeline() returned %s", Utils::toString(res).c_str());
		}

		return { std::move(vk::Pipeline(aWindow.device->device, gBufWritePipe	)), std::move(vk::Pipeline(aWindow.device->device, shadingPipe)) };
	}

	vk::Pipeline createShadowOffscreenPipeline(const VulkanWindow& aWindow, VkRenderPass aRenderPass, VkPipelineLayout aPipelineLayout) {
		vk::ShaderModule vert = loadShaderModule(aWindow, Shaders::shadowVert);

		VkPipelineShaderStageCreateInfo stages[1]{};
		stages[0].sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		stages[0].stage = VK_SHADER_STAGE_VERTEX_BIT;
		stages[0].module = vert.handle;
		stages[0].pName = "main";

		VkVertexInputBindingDescription vertexInputs[1]{};
		vertexInputs[0].binding = 0;
		vertexInputs[0].stride = sizeof(float) * 3;
		vertexInputs[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;

		VkVertexInputAttributeDescription vertexAttributes[1]{};
		vertexAttributes[0].binding = 0;
		vertexAttributes[0].location = 0;
		vertexAttributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
		vertexAttributes[0].offset = 0;

		VkPipelineVertexInputStateCreateInfo inputInfo{};
		inputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
		inputInfo.vertexBindingDescriptionCount = 1;
		inputInfo.pVertexBindingDescriptions = vertexInputs;
		inputInfo.vertexAttributeDescriptionCount = 1;
		inputInfo.pVertexAttributeDescriptions = vertexAttributes;

		VkPipelineInputAssemblyStateCreateInfo assemblyInfo{};
		assemblyInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
		assemblyInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
		assemblyInfo.primitiveRestartEnable = VK_FALSE;

		VkViewport viewport{};
		viewport.x = 0.0f;
		viewport.y = 0.0f;
		viewport.width = 2048.0f;
		viewport.height = 2048.0f;
		viewport.minDepth = 0.0f;
		viewport.maxDepth = 1.0f;

		VkRect2D scissor{};
		scissor.offset = VkOffset2D{ 0, 0 };
		scissor.extent = VkExtent2D{ 2048, 2048 };

		VkPipelineViewportStateCreateInfo viewportInfo{};
		viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportInfo.viewportCount = 1;
		viewportInfo.pViewports = &viewport;
		viewportInfo.scissorCount = 1;
		viewportInfo.pScissors = &scissor;

		VkPipelineRasterizationStateCreateInfo rasterInfo{};
		rasterInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterInfo.depthClampEnable = VK_FALSE;
		rasterInfo.rasterizerDiscardEnable = VK_FALSE;
		rasterInfo.polygonMode = VK_POLYGON_MODE_FILL;
		rasterInfo.cullMode = VK_CULL_MODE_NONE;
		rasterInfo.depthBiasEnable = VK_TRUE;
		rasterInfo.lineWidth = 1.0f;

		VkPipelineMultisampleStateCreateInfo samplingInfo{};
		samplingInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		samplingInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

		VkPipelineColorBlendAttachmentState blendStates[1]{};
		blendStates[0].blendEnable = VK_FALSE;
		blendStates[0].colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;

		VkPipelineColorBlendStateCreateInfo blendInfo{};
		blendInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		blendInfo.logicOpEnable = VK_FALSE;
		blendInfo.attachmentCount = 0;
		blendInfo.pAttachments = blendStates;

		VkPipelineDepthStencilStateCreateInfo depthInfo{};
		depthInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthInfo.depthTestEnable = VK_TRUE;
		depthInfo.depthWriteEnable = VK_TRUE;
		depthInfo.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
		depthInfo.minDepthBounds = 0.0f;
		depthInfo.maxDepthBounds = 1.0f;

		VkDynamicState dynamicStates {
			VK_DYNAMIC_STATE_DEPTH_BIAS
		};

		VkPipelineDynamicStateCreateInfo dynamicStateInfo{};
		dynamicStateInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
		dynamicStateInfo.dynamicStateCount = 1;
		dynamicStateInfo.pDynamicStates = &dynamicStates;

		VkGraphicsPipelineCreateInfo pipeInfo{};
		pipeInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
		pipeInfo.stageCount = 1;
		pipeInfo.pStages = stages;
		pipeInfo.pVertexInputState = &inputInfo;
		pipeInfo.pInputAssemblyState = &assemblyInfo;
		pipeInfo.pTessellationState = nullptr;
		pipeInfo.pViewportState = &viewportInfo;
		pipeInfo.pRasterizationState = &rasterInfo;
		pipeInfo.pMultisampleState = &samplingInfo;
		pipeInfo.pDepthStencilState = &depthInfo;
		pipeInfo.pColorBlendState = &blendInfo;
		pipeInfo.pDynamicState = &dynamicStateInfo;
		pipeInfo.layout = aPipelineLayout;
		pipeInfo.renderPass = aRenderPass;
		pipeInfo.subpass = 0;

		VkPipeline pipe = VK_NULL_HANDLE;
		if (const auto res = vkCreateGraphicsPipelines(aWindow.device->device, VK_NULL_HANDLE, 1, &pipeInfo, nullptr, &pipe); VK_SUCCESS != res) {
			throw Utils::Error("Unable to create graphics pipeline\n vkCreateGraphicsPipeline() returned %s", Utils::toString(res).c_str());
		}

		return vk::Pipeline(aWindow.device->device, pipe);
	}

	// Should only be used for render pass attachments
	std::pair<vk::Texture, vk::ImageView> createTextureBuffer(const VulkanContext& aContext, TextureBufferSetting aBufferSetting) {
		VkImageCreateInfo imageInfo{};
		imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
		imageInfo.imageType = VK_IMAGE_TYPE_2D;
		imageInfo.format = aBufferSetting.imageFormat;
		imageInfo.extent.width = aBufferSetting.extent.width;
		imageInfo.extent.height = aBufferSetting.extent.height;
		imageInfo.extent.depth = 1;
		imageInfo.mipLevels = 1;
		imageInfo.arrayLayers = 1;
		imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;
		imageInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
		imageInfo.usage = aBufferSetting.imageUsage;
		imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;

		VmaAllocationCreateInfo allocInfo{};
		allocInfo.usage = VMA_MEMORY_USAGE_GPU_ONLY;

		VkImage image = VK_NULL_HANDLE;
		VmaAllocation allocation = VK_NULL_HANDLE;

		if (const auto res = vmaCreateImage(aContext.allocator->allocator, &imageInfo, &allocInfo, &image, &allocation, nullptr); VK_SUCCESS != res)
			throw Utils::Error("Unable to allocate depth buffer image.\n vmaCreateImage() returned %s", Utils::toString(res).c_str());

		vk::Texture Image(aContext.allocator->allocator, "depth", image, allocation);

		VkImageViewCreateInfo viewInfo{};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = Image.image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = aBufferSetting.imageFormat;
		viewInfo.components = VkComponentMapping{};
		viewInfo.subresourceRange = VkImageSubresourceRange{ aBufferSetting.imageAspectFlags, 0, 1, 0, 1 };

		VkImageView view = VK_NULL_HANDLE;
		if (const auto res = vkCreateImageView(aContext.window->device->device, &viewInfo, nullptr, &view); VK_SUCCESS != res)
			throw Utils::Error("Unable to create image view.\n vkCreateImageView() returned %s", Utils::toString(res).c_str());

		return { std::move(Image), vk::ImageView(aContext.window->device->device, view) };
	}

	void createFramebuffers(const VulkanWindow& aWindow, std::vector<vk::Framebuffer>& aFramebuffers, VkRenderPass aRenderPass, std::vector<VkImageView>& aImageViews, VkExtent2D aExtent, bool ignoreSwapchainImage) {
		assert(aFramebuffers.empty());

		for (std::size_t i = 0; i < aWindow.swapViews.size(); ++i) {
			std::vector<VkImageView> attachments;
			if (!ignoreSwapchainImage)
				attachments.push_back(aWindow.swapViews[i]);

			for (std::size_t j = 0; j < aImageViews.size(); j++) {
				attachments.push_back(aImageViews[j]);
			}

			VkFramebufferCreateInfo fbInfo{};
			fbInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
			fbInfo.flags = 0;
			fbInfo.renderPass = aRenderPass;
			fbInfo.attachmentCount = attachments.size();
			fbInfo.pAttachments = attachments.data();
			fbInfo.width = aExtent.width;
			fbInfo.height = aExtent.height;
			fbInfo.layers = 1;

			VkFramebuffer fb = VK_NULL_HANDLE;
			if (const auto res = vkCreateFramebuffer(aWindow.device->device, &fbInfo, nullptr, &fb); VK_SUCCESS != res)
				throw Utils::Error("Unable to create framebuffer for swap chain image %zu\n vkCreateFramebuffer() returned %s", i, Utils::toString(res).c_str());

			aFramebuffers.emplace_back(vk::Framebuffer(aWindow.device->device, fb));
		}

		assert(aWindow.swapViews.size() == aFramebuffers.size());
	}

	VkDescriptorSet createUBODescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer) {
		VkDescriptorSet uboDescriptor = Engine::allocateDescriptorSet(aWindow, aWindow.device->dPool, aSetLayout);
		{
			VkWriteDescriptorSet desc[1]{};

			VkDescriptorBufferInfo uboInfo{};
			uboInfo.buffer = aBuffer;
			uboInfo.range = VK_WHOLE_SIZE;

			desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[0].dstSet = uboDescriptor;
			desc[0].dstBinding = 0;
			desc[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			desc[0].descriptorCount = 1;
			desc[0].pBufferInfo = &uboInfo;

			constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
			vkUpdateDescriptorSets(aWindow.device->device, numSets, desc, 0, nullptr);
		}

		return uboDescriptor;
	}

	VkDescriptorSet createModelMatricesDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkBuffer aBuffer, VkDeviceSize dynamicAlignment) {
		// Create dynamic UBO for model matrices
		VkDescriptorSet modelMatricesDescriptor = allocateDescriptorSet(aWindow, aWindow.device->dPool, aSetLayout);
		{
			VkWriteDescriptorSet desc[1]{};

			VkDescriptorBufferInfo modelMatricesInfo{};
			modelMatricesInfo.buffer = aBuffer;
			modelMatricesInfo.range = dynamicAlignment;

			desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[0].dstSet = modelMatricesDescriptor;
			desc[0].dstBinding = 0;
			desc[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
			desc[0].descriptorCount = 1;
			desc[0].pBufferInfo = &modelMatricesInfo;

			constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
			vkUpdateDescriptorSets(aWindow.device->device, numSets, desc, 0, nullptr);
		}

		return modelMatricesDescriptor;
	}

	VkDescriptorSet createDeferredShadingDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkImageView aDepthView, VkImageView aNormalsView, VkImageView aAlbedoView, VkImageView aEmissiveView) {
		// Create deferred shading descriptor
		VkDescriptorSet deferredShadingDescriptor = allocateDescriptorSet(aWindow, aWindow.device->dPool, aSetLayout);
		{
			VkWriteDescriptorSet desc[4]{};

			VkDescriptorImageInfo inputAttachments[4]{};
			inputAttachments[0].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			inputAttachments[0].imageView = aNormalsView;
			inputAttachments[0].sampler = VK_NULL_HANDLE;

			inputAttachments[1].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			inputAttachments[1].imageView = aAlbedoView;
			inputAttachments[1].sampler = VK_NULL_HANDLE;

			inputAttachments[2].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			inputAttachments[2].imageView = aEmissiveView;
			inputAttachments[2].sampler = VK_NULL_HANDLE;

			inputAttachments[3].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
			inputAttachments[3].imageView = aDepthView;
			inputAttachments[3].sampler = VK_NULL_HANDLE;

			desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[0].dstSet = deferredShadingDescriptor;
			desc[0].dstBinding = 0;
			desc[0].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			desc[0].descriptorCount = 1;
			desc[0].pImageInfo = &inputAttachments[0];

			desc[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[1].dstSet = deferredShadingDescriptor;
			desc[1].dstBinding = 1;
			desc[1].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			desc[1].descriptorCount = 1;
			desc[1].pImageInfo = &inputAttachments[1];

			desc[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[2].dstSet = deferredShadingDescriptor;
			desc[2].dstBinding = 2;
			desc[2].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			desc[2].descriptorCount = 1;
			desc[2].pImageInfo = &inputAttachments[2];

			desc[3].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[3].dstSet = deferredShadingDescriptor;
			desc[3].dstBinding = 3;
			desc[3].descriptorType = VK_DESCRIPTOR_TYPE_INPUT_ATTACHMENT;
			desc[3].descriptorCount = 1;
			desc[3].pImageInfo = &inputAttachments[3];

			constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
			vkUpdateDescriptorSets(aWindow.device->device, numSets, desc, 0, nullptr);
		}

		return deferredShadingDescriptor;
	}

	// Only works for shadow depth image at the moment
	VkDescriptorSet createImageDescriptor(const VulkanWindow& aWindow, VkDescriptorSetLayout aSetLayout, VkImageView aImageView, VkSampler aSampler) {
		VkDescriptorSet imageDescriptor = allocateDescriptorSet(aWindow, aWindow.device->dPool, aSetLayout);
		{
			VkWriteDescriptorSet desc[1]{};

			VkDescriptorImageInfo imageInfo[1]{};
			imageInfo[0].imageLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_READ_ONLY_OPTIMAL;
			imageInfo[0].imageView = aImageView;
			imageInfo[0].sampler = aSampler;

			desc[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			desc[0].dstSet = imageDescriptor;
			desc[0].dstBinding = 0;
			desc[0].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
			desc[0].descriptorCount = 1;
			desc[0].pImageInfo = imageInfo;

			constexpr auto numSets = sizeof(desc) / sizeof(desc[0]);
			vkUpdateDescriptorSets(aWindow.device->device, numSets, desc, 0, nullptr);
		}

		return imageDescriptor;
	}
}
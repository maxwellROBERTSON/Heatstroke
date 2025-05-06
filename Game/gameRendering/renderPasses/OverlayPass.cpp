#include "OverlayPass.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "Engine/vulkan/VulkanWindow.hpp"
#include "Engine/vulkan/VulkanDevice.hpp"

OverlayPass::OverlayPass(Engine::VulkanWindow* window, VkSampleCountFlagBits* sampleCount) : HsRenderPass(window) {
	this->sampleCount = sampleCount;

	this->recreate();
}

void OverlayPass::recreate() {
	if (*this->sampleCount & VK_SAMPLE_COUNT_1_BIT) {
		this->recreateNonMSAA();
	}
	else {
		this->recreateMSAA();
	}
}

void OverlayPass::recreateNonMSAA() {
	VkAttachmentDescription attachments[2]{};
	attachments[0].format = this->window->swapchainFormat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	attachments[1].format = VK_FORMAT_D32_SFLOAT_S8_UINT;
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
	if (const auto res = vkCreateRenderPass(this->window->device->device, &passInfo, nullptr, &rpass); VK_SUCCESS != res) {
		throw Utils::Error("Unable to create render pass\n vkCreateRenderPass() returned %s\n", Utils::toString(res).c_str());
	}

	this->renderPass = Engine::vk::RenderPass(this->window->device->device, rpass);

	VkClearValue colourClearValue{};
	colourClearValue.color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	VkClearValue depthClearValue{};
	depthClearValue.depthStencil.depth = 1.0f;

	this->clearValues.clear();
	this->clearValues.emplace_back(colourClearValue);
	this->clearValues.emplace_back(depthClearValue);
}

void OverlayPass::recreateMSAA() {
	VkAttachmentDescription attachments[3]{};
	// Framebuffer attachment in which multisampled colour attachment will be resolved to.
	attachments[0].format = this->window->swapchainFormat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	// Multisampled colour attachment
	attachments[1].format = this->window->swapchainFormat;
	attachments[1].samples = *this->sampleCount;
	attachments[1].loadOp = VK_ATTACHMENT_LOAD_OP_LOAD;
	attachments[1].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[1].initialLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;
	attachments[1].finalLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// Mutlisampled depth attachment
	attachments[2].format = VK_FORMAT_D32_SFLOAT_S8_UINT;
	attachments[2].samples = *this->sampleCount;
	attachments[2].loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
	attachments[2].storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	attachments[2].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[2].finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;
	attachments[2].stencilLoadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;

	// The attachment we are resolving to (the swapchain attachment)
	VkAttachmentReference resolveAttachment{};
	resolveAttachment.attachment = 0;
	resolveAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// The multisampled colour attachment
	VkAttachmentReference colourAttachment{};
	colourAttachment.attachment = 1;
	colourAttachment.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	// The multisampled depth attachment
	VkAttachmentReference depthAttachment{};
	depthAttachment.attachment = 2;
	depthAttachment.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpasses[1]{};
	subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[0].colorAttachmentCount = 1;
	subpasses[0].pResolveAttachments = &resolveAttachment;
	subpasses[0].pColorAttachments = &colourAttachment;
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
	passInfo.attachmentCount = 3;
	passInfo.pAttachments = attachments;
	passInfo.subpassCount = 1;
	passInfo.pSubpasses = subpasses;
	passInfo.dependencyCount = 2;
	passInfo.pDependencies = deps;

	VkRenderPass rpass = VK_NULL_HANDLE;
	if (const auto res = vkCreateRenderPass(this->window->device->device, &passInfo, nullptr, &rpass); VK_SUCCESS != res) {
		throw Utils::Error("Unable to create render pass\n vkCreateRenderPass() returned %s\n", Utils::toString(res).c_str());
	}

	this->renderPass = Engine::vk::RenderPass(this->window->device->device, rpass);

	VkClearValue colourClearValue{};
	colourClearValue.color = { {0.0f, 0.0f, 0.0f, 1.0f} };
	VkClearValue depthClearValue{};
	depthClearValue.depthStencil.depth = 1.0f;

	this->clearValues.clear();
	this->clearValues.emplace_back(colourClearValue);
	this->clearValues.emplace_back(colourClearValue);
	this->clearValues.emplace_back(depthClearValue);
}
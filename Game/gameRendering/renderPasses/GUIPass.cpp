#include "GUIPass.hpp"

#include "Error.hpp"
#include "toString.hpp"
#include "Engine/vulkan/VulkanWindow.hpp"
#include "Engine/vulkan/VulkanDevice.hpp"

GUIPass::GUIPass(Engine::VulkanWindow* window, VkSampleCountFlagBits* sampleCount) : HsRenderPass(window) {
	this->sampleCount = sampleCount;

	this->recreate();
}

void GUIPass::recreate() {
	VkAttachmentDescription attachments[1]{};
	attachments[0].format = this->window->swapchainFormat;
	attachments[0].samples = VK_SAMPLE_COUNT_1_BIT;
	attachments[0].loadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	attachments[0].storeOp = VK_ATTACHMENT_STORE_OP_STORE;
	attachments[0].initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	attachments[0].finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;

	VkAttachmentReference subpassAttachments[1]{};
	subpassAttachments[0].attachment = 0;
	subpassAttachments[0].layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpasses[1]{};
	subpasses[0].pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpasses[0].colorAttachmentCount = 1;
	subpasses[0].pColorAttachments = subpassAttachments;

	VkSubpassDependency deps[1]{};
	deps[0].dependencyFlags = VK_DEPENDENCY_BY_REGION_BIT;
	deps[0].srcSubpass = VK_SUBPASS_EXTERNAL;
	deps[0].srcAccessMask = 0;
	deps[0].srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	deps[0].dstSubpass = 0;
	deps[0].dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
	deps[0].dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

	VkRenderPassCreateInfo passInfo{};
	passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	passInfo.attachmentCount = 1;
	passInfo.pAttachments = attachments;
	passInfo.subpassCount = 1;
	passInfo.pSubpasses = subpasses;
	passInfo.dependencyCount = 1;
	passInfo.pDependencies = deps;

	VkRenderPass rpass = VK_NULL_HANDLE;
	if (const auto res = vkCreateRenderPass(this->window->device->device, &passInfo, nullptr, &rpass); VK_SUCCESS != res) {
		throw Utils::Error("Unable to create render pass\n vkCreateRenderPass() returned %s\n", Utils::toString(res).c_str());
	}

	this->renderPass = Engine::vk::RenderPass(this->window->device->device, rpass);

	// Since clear values are determined by the attachments of the render pass
	// we create the clear values here
	VkClearValue colourClearValue{};
	colourClearValue.color = { {0.0f, 0.0f, 0.0f, 1.0f} };

	this->clearValues.clear();
	this->clearValues.emplace_back(colourClearValue);
}
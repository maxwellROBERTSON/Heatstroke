#include "HsRenderPass.hpp"

namespace Engine {

	HsRenderPass::HsRenderPass(VulkanWindow* window) {
		this->window = window;
	}

	void HsRenderPass::recreate() {};

	vk::RenderPass& HsRenderPass::getRenderPass() {
		return this->renderPass;
	}

	VkRenderPass HsRenderPass::getRenderPassHandle() {
		return this->renderPass.handle;
	}

	std::vector<VkClearValue>& HsRenderPass::getClearValues() {
		return this->clearValues;
	}

}
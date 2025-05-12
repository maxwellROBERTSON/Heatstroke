#include "GUIFramebuffer.hpp"

#include "../../../PipelineCreation.hpp"

GUIFramebuffer::GUIFramebuffer(
	Engine::VulkanWindow* window,
	std::map<std::string, TextureBuffer>* textureBuffers,
	Engine::HsRenderPass* renderPass) : HsFramebuffer(window) {
	this->textureBuffers = textureBuffers;
	this->renderPass = renderPass;

	this->renderExtent = &this->window->swapchainExtent;

	this->recreate();
}

void GUIFramebuffer::recreate() {
	this->framebuffers.clear();

	std::vector<VkImageView> swapchainViews;
	Engine::createFramebuffers(*this->window, this->framebuffers, this->renderPass->getHandle(), swapchainViews, *this->renderExtent);
}

#include "GUIFramebuffer.hpp"

#include "Engine/vulkan/PipelineCreation.hpp"

GUIFramebuffer::GUIFramebuffer(
	Engine::VulkanWindow* window,
	std::map<std::string, TextureBuffer>* textureBuffers,
	Engine::HsRenderPass* renderPass) : HsFramebuffer(window) {
	this->textureBuffers = textureBuffers;
	this->renderPass = renderPass;

	this->recreate();
}

void GUIFramebuffer::recreate() {
	this->framebuffers.clear();

	VkExtent2D extent = this->window->swapchainExtent;

	std::vector<VkImageView> swapchainViews;
	Engine::createFramebuffers(*this->window, this->framebuffers, this->renderPass->getRenderPass().handle, swapchainViews, extent);
}

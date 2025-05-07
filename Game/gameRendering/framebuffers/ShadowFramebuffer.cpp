#include "ShadowFramebuffer.hpp"

#include "Engine/vulkan/PipelineCreation.hpp"

ShadowFramebuffer::ShadowFramebuffer(
	Engine::VulkanWindow* window,
	std::map<std::string, TextureBuffer>* textureBuffers,
	Engine::HsRenderPass* renderPass,
	VkExtent2D* shadowMapResolution) : HsFramebuffer(window) {
	this->textureBuffers = textureBuffers;
	this->renderPass = renderPass;

	this->shadowMapResolution = shadowMapResolution;

	this->recreate();
}

void ShadowFramebuffer::recreate() {
	this->framebuffers.clear();

	VkExtent2D extent = *this->shadowMapResolution;

	std::vector<VkImageView> shadowViews;
	shadowViews.emplace_back(this->textureBuffers->at("shadowDepth")->getImageView().handle);
	Engine::createFramebuffers(*this->window, this->framebuffers, this->renderPass->getRenderPass().handle, shadowViews, extent, true);
}

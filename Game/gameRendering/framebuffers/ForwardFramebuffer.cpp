#include "ForwardFramebuffer.hpp"

#include "Engine/vulkan/PipelineCreation.hpp"

ForwardFramebuffer::ForwardFramebuffer(
	Engine::VulkanWindow* window, 
	std::map<std::string, TextureBuffer>* textureBuffers,
	Engine::HsRenderPass* renderPass,
	VkSampleCountFlagBits* sampleCount) : HsFramebuffer(window) {
	this->textureBuffers = textureBuffers;
	this->renderPass = renderPass;
	this->sampleCount = sampleCount;
	
	this->recreate();
}

void ForwardFramebuffer::recreate() {
	this->framebuffers.clear();

	bool usingMSAA = !(*this->sampleCount & VK_SAMPLE_COUNT_1_BIT);
	VkExtent2D extent = this->window->swapchainExtent;

	std::vector<VkImageView> forwardViews;
	forwardViews.emplace_back(this->textureBuffers->at("depth")->getImageView().handle);

	std::vector<VkImageView> forwardMSAAViews;
	forwardMSAAViews.emplace_back(this->textureBuffers->at("multisampleColour")->getImageView().handle);
	forwardMSAAViews.emplace_back(this->textureBuffers->at("multisampleDepth")->getImageView().handle);

	Engine::createFramebuffers(*this->window, this->framebuffers, this->renderPass->getRenderPass().handle, usingMSAA ? forwardMSAAViews : forwardViews, extent);	
}

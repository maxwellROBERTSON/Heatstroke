#include "DepthTextureBuffer.hpp"

#include "Engine/vulkan/VulkanContext.hpp"
#include "Engine/vulkan/PipelineCreation.hpp"

DepthTextureBuffer::DepthTextureBuffer(
	Engine::VulkanContext* context, 
	VkSampleCountFlagBits* sampleCount) : HsTextureBuffer(context) {
	this->sampleCount = sampleCount;

	this->recreate();
}

void DepthTextureBuffer::recreate() {
	Engine::TextureBufferSetting depthTextureSetting = {
		.imageFormat = VK_FORMAT_D32_SFLOAT_S8_UINT,
		.imageExtent = this->context->window->swapchainExtent,
		.imageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_INPUT_ATTACHMENT_BIT,
		.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT };

	std::pair<Engine::vk::Texture, Engine::vk::ImageView> textureBuffer = Engine::createTextureBuffer(*this->context, depthTextureSetting);

	this->image = std::move(textureBuffer.first);
	this->imageView = std::move(textureBuffer.second);
}
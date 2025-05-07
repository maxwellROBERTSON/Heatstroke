#include "MultisampledDepthTextureBuffer.hpp"

#include "Engine/vulkan/VulkanContext.hpp"
#include "Engine/vulkan/PipelineCreation.hpp"

#include "VulkanUtils.hpp"

MultisampledDepthTextureBuffer::MultisampledDepthTextureBuffer(
	Engine::VulkanContext* context,
	VkSampleCountFlagBits* sampleCount) : HsTextureBuffer(context) {
	this->sampleCount = sampleCount;

	this->recreate();
}

void MultisampledDepthTextureBuffer::recreate() {
	Engine::TextureBufferSetting multisampledDepthTextureSetting = {
		.imageFormat = VK_FORMAT_D32_SFLOAT_S8_UINT,
		.imageExtent = this->context->window->swapchainExtent,
		.imageUsage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT,
		.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT,
		.samples = Utils::getMSAAMinimum(*this->sampleCount) };

	std::pair<Engine::vk::Texture, Engine::vk::ImageView> textureBuffer = Engine::createTextureBuffer(*this->context, multisampledDepthTextureSetting);

	this->image = std::move(textureBuffer.first);
	this->imageView = std::move(textureBuffer.second);
}
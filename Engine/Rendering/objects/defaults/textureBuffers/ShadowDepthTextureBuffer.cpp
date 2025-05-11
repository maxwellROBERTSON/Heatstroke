#include "ShadowDepthTextureBuffer.hpp"

#include "../../../../vulkan/VulkanContext.hpp"
#include "../../../PipelineCreation.hpp"

ShadowDepthTextureBuffer::ShadowDepthTextureBuffer(
	Engine::VulkanContext* context,
	VkSampleCountFlagBits* sampleCount,
	VkExtent2D* shadowMapResolution) : HsTextureBuffer(context) {
	this->sampleCount = sampleCount;

	this->shadowMapResolution = shadowMapResolution;

	this->recreate();
}

void ShadowDepthTextureBuffer::recreate() {
	Engine::TextureBufferSetting shadowDepthTextureSetting = {
		.imageFormat = VK_FORMAT_D32_SFLOAT_S8_UINT,
		.imageExtent = *this->shadowMapResolution,
		.imageUsage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
		.viewAspectFlags = VK_IMAGE_ASPECT_DEPTH_BIT };

	std::pair<Engine::vk::Texture, Engine::vk::ImageView> textureBuffer = Engine::createTextureBuffer(*this->context, shadowDepthTextureSetting);

	this->image = std::move(textureBuffer.first);
	this->imageView = std::move(textureBuffer.second);
}
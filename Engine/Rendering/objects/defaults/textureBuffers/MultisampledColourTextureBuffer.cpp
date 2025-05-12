#include "MultisampledColourTextureBuffer.hpp"

#include "../../../../vulkan/VulkanContext.hpp"
#include "../../../PipelineCreation.hpp"

#include "VulkanUtils.hpp"

MultisampledColourTextureBuffer::MultisampledColourTextureBuffer(
	Engine::VulkanContext* context,
	VkSampleCountFlagBits* sampleCount) : HsTextureBuffer(context) {
	this->sampleCount = sampleCount;

	this->recreate();
}

void MultisampledColourTextureBuffer::recreate() {
	Engine::TextureBufferSetting multisampledColourTextureSetting = {
		.imageFormat = this->context->window->swapchainFormat,
		.imageExtent = this->context->window->swapchainExtent,
		.imageUsage = VK_IMAGE_USAGE_TRANSIENT_ATTACHMENT_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT,
		.viewAspectFlags = VK_IMAGE_ASPECT_COLOR_BIT,
		.samples = Utils::getMSAAMinimum(*this->sampleCount) };

	std::pair<Engine::vk::Texture, Engine::vk::ImageView> textureBuffer = Engine::createTextureBuffer(*this->context, multisampledColourTextureSetting);

	this->image = std::move(textureBuffer.first);
	this->imageView = std::move(textureBuffer.second);
}
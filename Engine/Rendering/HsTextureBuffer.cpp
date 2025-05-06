#include "HsTextureBuffer.hpp"

namespace Engine {

	HsTextureBuffer::HsTextureBuffer(VulkanContext* context) {
		this->context = context;
	}

	void HsTextureBuffer::recreate() {}

	vk::Texture& HsTextureBuffer::getImage() {
		return this->image;
	}

	vk::ImageView& HsTextureBuffer::getImageView() {
		return this->imageView;
	}

}
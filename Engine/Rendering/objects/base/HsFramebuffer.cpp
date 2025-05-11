#include "HsFramebuffer.hpp"

namespace Engine {

	HsFramebuffer::HsFramebuffer(VulkanWindow* window) {
		this->window = window;
	}

	void HsFramebuffer::recreate() {}

	VkExtent2D HsFramebuffer::getRenderExtent() {
		return *this->renderExtent;
	}

	VkFramebuffer HsFramebuffer::getHandle(std::uint32_t imageIndex) {
		return this->framebuffers[imageIndex].handle;
	}

}
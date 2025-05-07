#include "HsFramebuffer.hpp"

namespace Engine {

	HsFramebuffer::HsFramebuffer(VulkanWindow* window) {
		this->window = window;
	}

	void HsFramebuffer::recreate() {}

	VkFramebuffer HsFramebuffer::getFramebufferHandle(std::uint32_t imageIndex) {
		return this->framebuffers[imageIndex].handle;
	}

}
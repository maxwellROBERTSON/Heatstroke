#pragma once

#include <vector>

#include "../vulkan/objects/VkObjects.hpp"

namespace Engine {

	class VulkanWindow;

	class HsFramebuffer {
	public:
		HsFramebuffer() = default;
		HsFramebuffer(VulkanWindow* window);

		virtual ~HsFramebuffer() = default;

		virtual void recreate();

		VkFramebuffer getFramebufferHandle(std::uint32_t imageIndex);
	protected:
		VulkanWindow* window;

		std::vector<vk::Framebuffer> framebuffers;
	};

}
#pragma once

#include <vector>
#include <cstdint>

#include "../../../vulkan/objects/VkObjects.hpp"

namespace Engine {

	class VulkanWindow;

	class HsFramebuffer {
	public:
		HsFramebuffer() = default;
		HsFramebuffer(VulkanWindow* window);

		virtual ~HsFramebuffer() = default;

		virtual void recreate();

		VkExtent2D getRenderExtent();
		VkFramebuffer getHandle(std::uint32_t imageIndex);
	protected:
		VulkanWindow* window;

		VkExtent2D* renderExtent = nullptr;

		std::vector<vk::Framebuffer> framebuffers;
	};

}
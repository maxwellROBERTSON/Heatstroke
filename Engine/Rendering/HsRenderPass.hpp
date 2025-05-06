#pragma once

#include <vector>

#include "../vulkan/objects/VkObjects.hpp"

namespace Engine {

	class VulkanWindow;

	class HsRenderPass {
	public:
		HsRenderPass() = default;
		HsRenderPass(VulkanWindow* window);

		virtual ~HsRenderPass() = default;

		virtual void recreate();

		vk::RenderPass& getRenderPass();
		VkRenderPass getRenderPassHandle();
		std::vector<VkClearValue>& getClearValues();
	protected:
		VulkanWindow* window;

		vk::RenderPass renderPass;
		std::vector<VkClearValue> clearValues;

		// Pointer to current sample count setting
		VkSampleCountFlagBits* sampleCount = nullptr;
	};

}
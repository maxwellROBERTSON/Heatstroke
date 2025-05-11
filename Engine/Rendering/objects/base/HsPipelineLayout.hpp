#pragma once

#include "../../../vulkan/objects/VkObjects.hpp"

namespace Engine {

	class VulkanWindow;

	class HsPipelineLayout {
	public:
		HsPipelineLayout() = default;
		HsPipelineLayout(VulkanWindow* window);

		virtual ~HsPipelineLayout() = default;

		virtual void recreate();

		VkPipelineLayout getHandle();
	protected:
		VulkanWindow* window;

		vk::PipelineLayout pipelineLayout;
	};

}
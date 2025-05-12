#pragma once

#include <memory>

#include "../../../vulkan/objects/VkObjects.hpp"
#include "HsPipelineLayout.hpp"

namespace Engine {

	class VulkanWindow;

	class HsPipeline {
	public:
		HsPipeline() = default;
		HsPipeline(VulkanWindow* window);

		virtual ~HsPipeline() = default;

		virtual void recreate();

		VkPipeline getHandle();

	protected:
		VulkanWindow* window;

		vk::Pipeline pipeline;
		std::unique_ptr<HsPipelineLayout>* pipelineLayout = nullptr;
		vk::RenderPass* renderPass = nullptr;

		VkExtent2D* renderExtent = nullptr;

		VkSampleCountFlagBits* sampleCount = nullptr;
	};

}
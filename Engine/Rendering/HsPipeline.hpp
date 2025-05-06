#pragma once

#include <memory>

#include "../vulkan/objects/VkObjects.hpp"
#include "../Rendering/HsPipelineLayout.hpp"

namespace Engine {

	class VulkanWindow;

	class HsPipeline {
	public:
		HsPipeline() = default;
		HsPipeline(VulkanWindow* window);

		virtual ~HsPipeline() = default;

		virtual void recreate();

		VkPipeline getPipelineHandle();

	protected:
		VulkanWindow* window;

		vk::Pipeline pipeline;
		std::unique_ptr<HsPipelineLayout>* pipelineLayout = nullptr;
		vk::RenderPass* renderPass = nullptr;

		VkSampleCountFlagBits* sampleCount = nullptr;
	};

}
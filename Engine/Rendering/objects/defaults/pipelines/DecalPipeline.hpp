#pragma once

#include "../../base/HsPipeline.hpp"

namespace Engine {
	class HsRenderPass;
}

using PipelineLayout = std::unique_ptr<Engine::HsPipelineLayout>;

class DecalPipeline : public Engine::HsPipeline {
public:
	DecalPipeline(
		Engine::VulkanWindow* window,
		PipelineLayout* pipelineLayout,
		Engine::HsRenderPass* renderPass,
		VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
};
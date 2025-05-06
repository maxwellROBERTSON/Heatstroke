#pragma once

#include "Engine/Rendering/HsPipeline.hpp"

namespace Engine {
	class HsRenderPass;
}

using PipelineLayout = std::unique_ptr<Engine::HsPipelineLayout>;

class CrosshairPipeline : public Engine::HsPipeline {
public:
	CrosshairPipeline(
		Engine::VulkanWindow* window,
		PipelineLayout* pipelineLayout,
		Engine::HsRenderPass* renderPass,
		VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
};
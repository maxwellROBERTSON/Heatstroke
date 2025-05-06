#pragma once

#include "Engine/Rendering/HsPipeline.hpp"

namespace Engine {
	class HsRenderPass;
}

using PipelineLayout = std::unique_ptr<Engine::HsPipelineLayout>;

class SkyboxPipeline : public Engine::HsPipeline {
public:
	SkyboxPipeline(
		Engine::VulkanWindow* window,
		PipelineLayout* pipelineLayout,
		Engine::HsRenderPass* renderPass,
		VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
};
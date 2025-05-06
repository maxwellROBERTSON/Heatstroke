#pragma once

#include "Engine/Rendering/HsPipeline.hpp"
#include "Engine/Rendering/HsPipelineLayout.hpp"

namespace Engine {
	class HsRenderPass;
}

using PipelineLayout = std::unique_ptr<Engine::HsPipelineLayout>;

class ShadowPipeline : public Engine::HsPipeline {
public:
	ShadowPipeline(
		Engine::VulkanWindow* window,
		PipelineLayout* pipelineLayout,
		Engine::HsRenderPass* renderPass,
		VkSampleCountFlagBits* sampleCount,
		VkExtent2D* shadowMapResolution);

	void recreate();
private:
	VkExtent2D* shadowMapResolution = nullptr;
};
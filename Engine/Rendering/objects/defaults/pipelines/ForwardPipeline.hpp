#pragma once

#include "../../base/HsPipeline.hpp"

namespace Engine {
	class HsRenderPass;
}

using PipelineLayout = std::unique_ptr<Engine::HsPipelineLayout>;

class ForwardPipeline : public Engine::HsPipeline {
public:
	ForwardPipeline(
		Engine::VulkanWindow* window, 
		PipelineLayout* pipelineLayout,
		Engine::HsRenderPass* renderPass,
		VkSampleCountFlagBits* sampleCount,
		bool* shadows);

	void recreate();
private:
	bool* shadows = nullptr;
};
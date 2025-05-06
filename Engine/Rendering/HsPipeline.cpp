#include "HsPipeline.hpp"

namespace Engine {

	HsPipeline::HsPipeline(VulkanWindow* window) {
		this->window = window;
	}

	void HsPipeline::recreate() {}

	VkPipeline HsPipeline::getPipelineHandle() {
		return this->pipeline.handle;
	}

}
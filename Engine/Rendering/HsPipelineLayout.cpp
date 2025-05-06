#include "HsPipelineLayout.hpp"

namespace Engine {

	HsPipelineLayout::HsPipelineLayout(VulkanWindow* window) {
		this->window = window;
	}

	void HsPipelineLayout::recreate() {}

	VkPipelineLayout HsPipelineLayout::getPipelineLayoutHandle() {
		return this->pipelineLayout.handle;
	}

}
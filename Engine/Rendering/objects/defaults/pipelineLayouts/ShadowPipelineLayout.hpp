#pragma once

#include <map>
#include <string>

#include "../../base/HsPipelineLayout.hpp"

class ShadowPipelineLayout : public Engine::HsPipelineLayout {
public:
	ShadowPipelineLayout(
		Engine::VulkanWindow* window,
		std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts);

	void recreate();
private:
	std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts;
};
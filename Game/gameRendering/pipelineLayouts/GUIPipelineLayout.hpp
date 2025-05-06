#pragma once

#include <map>
#include <string>

#include "Engine/Rendering/HsPipelineLayout.hpp"

class GUIPipelineLayout : public Engine::HsPipelineLayout {
public:
	GUIPipelineLayout(
		Engine::VulkanWindow* window,
		std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts);

	void recreate();
private:
	std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts;
};
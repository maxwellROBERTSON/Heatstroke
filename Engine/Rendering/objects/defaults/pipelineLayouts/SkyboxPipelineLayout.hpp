#pragma once

#include <map>
#include <string>

#include "../../base/HsPipelineLayout.hpp"

class SkyboxPipelineLayout : public Engine::HsPipelineLayout {
public:
	SkyboxPipelineLayout(
		Engine::VulkanWindow* window,
		std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts);

	void recreate();
private:
	std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts;
};
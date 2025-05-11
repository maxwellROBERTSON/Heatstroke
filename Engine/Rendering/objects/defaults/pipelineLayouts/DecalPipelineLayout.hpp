#pragma once

#include <map>
#include <string>

#include "../../base/HsPipelineLayout.hpp"

class DecalPipelineLayout : public Engine::HsPipelineLayout {
public:
	DecalPipelineLayout(
		Engine::VulkanWindow* window,
		std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts);

	void recreate();
private:
	std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts;
};
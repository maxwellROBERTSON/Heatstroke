#pragma once

#include <map>
#include <string>

#include "../../base/HsPipelineLayout.hpp"

class ForwardPipelineLayout : public Engine::HsPipelineLayout {
public:
	ForwardPipelineLayout(
		Engine::VulkanWindow* window, 
		std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts,
		bool* shadowsEnabled);

	void recreate();
private:
	std::map<std::string, Engine::vk::DescriptorSetLayout>* descriptorLayouts;
	bool* shadowsEnabled = nullptr;
};
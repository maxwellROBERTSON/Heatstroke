#pragma once

#include "../../base/HsRenderPass.hpp"

class GUIPass : public Engine::HsRenderPass {
public:
	GUIPass(Engine::VulkanWindow* window, VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
};
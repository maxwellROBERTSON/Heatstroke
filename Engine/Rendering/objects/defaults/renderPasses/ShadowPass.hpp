#pragma once

#include "../../base/HsRenderPass.hpp"

class ShadowPass : public Engine::HsRenderPass {
public:
	ShadowPass(Engine::VulkanWindow* window, VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
};
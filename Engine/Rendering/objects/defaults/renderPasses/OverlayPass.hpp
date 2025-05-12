#pragma once

#include "../../base/HsRenderPass.hpp"

class OverlayPass : public Engine::HsRenderPass {
public:
	OverlayPass(Engine::VulkanWindow* window, VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
	void recreateNonMSAA();
	void recreateMSAA();
};
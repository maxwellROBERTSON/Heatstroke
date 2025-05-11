#pragma once

#include "../../base/HsRenderPass.hpp"

class ForwardPass : public Engine::HsRenderPass {
public:
	ForwardPass(Engine::VulkanWindow* window, VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
	void recreateNonMSAA();
	void recreateMSAA();
};
#pragma once

#include "Engine/Rendering/HsRenderPass.hpp"

namespace Engine {
	class VulkanWindow;
}

class ForwardPass : public Engine::HsRenderPass {
public:
	ForwardPass(Engine::VulkanWindow* window, VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
	void recreateNonMSAA();
	void recreateMSAA();
};
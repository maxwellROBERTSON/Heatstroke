#pragma once

#include "Engine/Rendering/HsRenderPass.hpp"

namespace Engine {
	class VulkanWindow;
}

class OverlayPass : public Engine::HsRenderPass {
public:
	OverlayPass(Engine::VulkanWindow* window, VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
	void recreateNonMSAA();
	void recreateMSAA();
};
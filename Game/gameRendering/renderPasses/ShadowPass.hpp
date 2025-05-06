#pragma once

#include "Engine/Rendering/HsRenderPass.hpp"

namespace Engine {
	class VulkanWindow;
}

class ShadowPass : public Engine::HsRenderPass {
public:
	ShadowPass(Engine::VulkanWindow* window, VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
};
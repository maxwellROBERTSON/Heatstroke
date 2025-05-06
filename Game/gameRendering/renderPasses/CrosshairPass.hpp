#pragma once

#include "Engine/Rendering/HsRenderPass.hpp"

namespace Engine {
	class VulkanWindow;
}

class CrosshairPass : public Engine::HsRenderPass {
public:
	CrosshairPass(Engine::VulkanWindow* window, VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
};
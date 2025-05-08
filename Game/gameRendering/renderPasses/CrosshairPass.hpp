#pragma once

#include "Engine/Rendering/objects/base/HsRenderPass.hpp"

namespace Engine {
	class VulkanWindow;
}

class CrosshairPass : public Engine::HsRenderPass {
public:
	CrosshairPass(Engine::VulkanWindow* window, VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
};
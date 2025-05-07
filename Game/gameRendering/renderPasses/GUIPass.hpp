#pragma once

#include "Engine/Rendering/HsRenderPass.hpp"

namespace Engine {
	class VulkanWindow;
}

class GUIPass : public Engine::HsRenderPass {
public:
	GUIPass(Engine::VulkanWindow* window, VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
};
#pragma once

#include "Engine/Rendering/HsTextureBuffer.hpp"

class MultisampledDepthTextureBuffer : public Engine::HsTextureBuffer {
public:
	MultisampledDepthTextureBuffer(
		Engine::VulkanContext* context,
		VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
};
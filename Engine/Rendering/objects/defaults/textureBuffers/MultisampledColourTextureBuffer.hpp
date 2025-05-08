#pragma once

#include "../../base/HsTextureBuffer.hpp"

class MultisampledColourTextureBuffer : public Engine::HsTextureBuffer {
public:
	MultisampledColourTextureBuffer(
		Engine::VulkanContext* context,
		VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
};
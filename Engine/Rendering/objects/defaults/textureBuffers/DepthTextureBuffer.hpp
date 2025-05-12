#pragma once

#include "../../base/HsTextureBuffer.hpp"

class DepthTextureBuffer : public Engine::HsTextureBuffer {
public:
	DepthTextureBuffer(Engine::VulkanContext* context, VkSampleCountFlagBits* sampleCount);

	void recreate();
private:

};
#pragma once

#include "Engine/Rendering/HsTextureBuffer.hpp"

class DepthTextureBuffer : public Engine::HsTextureBuffer {
public:
	DepthTextureBuffer(Engine::VulkanContext* context, VkSampleCountFlagBits* sampleCount);

	void recreate();
private:

};
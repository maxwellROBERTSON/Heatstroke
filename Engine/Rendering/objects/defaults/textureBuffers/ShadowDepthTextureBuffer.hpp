#pragma once

#include "../../base/HsTextureBuffer.hpp"

class ShadowDepthTextureBuffer : public Engine::HsTextureBuffer {
public:
	ShadowDepthTextureBuffer(
		Engine::VulkanContext* context,
		VkSampleCountFlagBits* sampleCount, 
		VkExtent2D* shadowMapResolution);

	void recreate();
private:
	VkExtent2D* shadowMapResolution = nullptr;
};
#pragma once

#include <map>
#include <string>
#include <memory>

#include "Engine/Rendering/HsFramebuffer.hpp"
#include "Engine/Rendering/HsTextureBuffer.hpp"
#include "Engine/Rendering/HsRenderPass.hpp"

using TextureBuffer = std::unique_ptr<Engine::HsTextureBuffer>;

class ForwardFramebuffer : public Engine::HsFramebuffer {
public:
	ForwardFramebuffer(
		Engine::VulkanWindow* window, 
		std::map<std::string, TextureBuffer>* textureBuffers,
		Engine::HsRenderPass* renderPass,
		VkSampleCountFlagBits* sampleCount);

	void recreate();
private:
	std::map<std::string, TextureBuffer>* textureBuffers;
	Engine::HsRenderPass* renderPass;
	VkSampleCountFlagBits* sampleCount;
};
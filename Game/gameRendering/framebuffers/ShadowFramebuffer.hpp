#pragma once

#include <map>
#include <string>
#include <memory>

#include "Engine/Rendering/HsFramebuffer.hpp"
#include "Engine/Rendering/HsTextureBuffer.hpp"
#include "Engine/Rendering/HsRenderPass.hpp"

using TextureBuffer = std::unique_ptr<Engine::HsTextureBuffer>;

class ShadowFramebuffer : public Engine::HsFramebuffer {
public:
	ShadowFramebuffer(
		Engine::VulkanWindow* window,
		std::map<std::string, TextureBuffer>* textureBuffers,
		Engine::HsRenderPass* renderPass,
		VkExtent2D* shadowMapResolution);

	void recreate();
private:
	std::map<std::string, TextureBuffer>* textureBuffers;
	Engine::HsRenderPass* renderPass;

	VkExtent2D* shadowMapResolution = nullptr;
};
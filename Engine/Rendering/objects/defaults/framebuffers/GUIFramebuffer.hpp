#pragma once

#include <map>
#include <string>
#include <memory>

#include "../../base/HsFramebuffer.hpp"
#include "../../base/HsTextureBuffer.hpp"
#include "../../base/HsRenderPass.hpp"

using TextureBuffer = std::unique_ptr<Engine::HsTextureBuffer>;

class GUIFramebuffer : public Engine::HsFramebuffer {
public:
	GUIFramebuffer(
		Engine::VulkanWindow* window,
		std::map<std::string, TextureBuffer>* textureBuffers,
		Engine::HsRenderPass* renderPass);

	void recreate();
private:
	std::map<std::string, TextureBuffer>* textureBuffers;
	Engine::HsRenderPass* renderPass;
};
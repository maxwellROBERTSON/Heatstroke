#pragma once

#include "Engine/Rendering/HsUniformBuffer.hpp"

#include "Uniforms.hpp"

class SceneUniformBuffer : public Engine::HsUniformBuffer {
public:
	SceneUniformBuffer(Engine::VulkanAllocator* allocator, glsl::SceneUniform* sceneUniform);

	void update(VkCommandBuffer cmdBuf);
private:
	glsl::SceneUniform* uniformData;
};
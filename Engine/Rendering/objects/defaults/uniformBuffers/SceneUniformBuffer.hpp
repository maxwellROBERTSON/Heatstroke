#pragma once

#include "../../base/HsUniformBuffer.hpp"

#include "Uniforms.hpp"

class SceneUniformBuffer : public Engine::HsUniformBuffer {
public:
	SceneUniformBuffer(Engine::VulkanAllocator* allocator, glsl::SceneUniform* sceneUniform);

	void update(VkCommandBuffer cmdBuf);
private:
	glsl::SceneUniform* uniformData;
};
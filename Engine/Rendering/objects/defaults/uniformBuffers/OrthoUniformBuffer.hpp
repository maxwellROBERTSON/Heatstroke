#pragma once

#include "../../base/HsUniformBuffer.hpp"

#include "Uniforms.hpp"

class OrthoUniformBuffer : public Engine::HsUniformBuffer {
public:
	OrthoUniformBuffer(Engine::VulkanAllocator* allocator, glsl::OrthoMatrices* orthoMatrices);

	void update(VkCommandBuffer cmdBuf);
private:
	glsl::OrthoMatrices* uniformData;
};
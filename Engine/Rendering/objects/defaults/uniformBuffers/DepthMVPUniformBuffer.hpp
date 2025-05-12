#pragma once

#include "../../base/HsUniformBuffer.hpp"

#include "Uniforms.hpp"

class DepthMVPUniformBuffer : public Engine::HsUniformBuffer {
public:
	DepthMVPUniformBuffer(Engine::VulkanAllocator* allocator, glsl::DepthMVP* depthMVP);

	void update(VkCommandBuffer cmdBuf);
private:
	glsl::DepthMVP* uniformData;
};
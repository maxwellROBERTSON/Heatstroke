#pragma once

#include "objects/base/HsRenderPass.hpp"
#include "objects/base/HsFramebuffer.hpp"

#include <volk/volk.h>

namespace Engine {

	void beginRenderPass(VkCommandBuffer cmdBuf, HsRenderPass* renderPass, HsFramebuffer* framebuffer, std::uint32_t imageIndex);
	void endRenderPass(VkCommandBuffer cmdBuf);
}
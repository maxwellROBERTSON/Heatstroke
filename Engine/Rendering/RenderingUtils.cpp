#include "RenderingUtils.hpp"

namespace Engine {

	void beginRenderPass(VkCommandBuffer cmdBuf, HsRenderPass* renderPass, HsFramebuffer* framebuffer, std::uint32_t imageIndex) {
		VkRenderPassBeginInfo passInfo{};
		passInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		passInfo.renderPass = renderPass->getHandle();
		passInfo.framebuffer = framebuffer->getHandle(imageIndex);
		passInfo.renderArea.offset = VkOffset2D{ 0, 0 };
		passInfo.renderArea.extent = framebuffer->getRenderExtent();
		passInfo.clearValueCount = static_cast<std::uint32_t>(renderPass->getClearValues().size());
		passInfo.pClearValues = renderPass->getClearValues().data();

		vkCmdBeginRenderPass(cmdBuf, &passInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	void endRenderPass(VkCommandBuffer cmdBuf) {
		vkCmdEndRenderPass(cmdBuf);
	}

}
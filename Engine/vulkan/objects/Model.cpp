#include "Model.hpp"

namespace Engine {
namespace vk {

	void Model::drawModel(VkCommandBuffer aCmdBuf) {

		VkBuffer vBuffers[4] = {
			posBuffer.buffer,
			normBuffer.buffer,
			texBuffer.buffer,
			vertColBuffer.buffer
		};
		VkBuffer iBuffer = indicesBuffer.buffer;
		VkDeviceSize vOffsets[4]{};
		VkDeviceSize iOffset{};

		vkCmdBindVertexBuffers(aCmdBuf, 0, 4, vBuffers, vOffsets);
		vkCmdBindIndexBuffer(aCmdBuf, iBuffer, iOffset, VK_INDEX_TYPE_UINT32);

		for (Node* node : nodes) {
			drawNode(node, aCmdBuf);
		}

	}

	void Model::drawNode(Node* aNode, VkCommandBuffer aCmdBuf) {
		if (aNode->mesh) {
			for (Primitive* primitive : aNode->mesh->primitives)
				vkCmdDrawIndexed(aCmdBuf, primitive->indexCount, 1, primitive->firstIndex, 0, 0);
		}

		for (Node* child : aNode->children) {
			drawNode(child, aCmdBuf);
		}
	}

}
}
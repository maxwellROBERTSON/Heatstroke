#include "Node.hpp"

#include "Skin.hpp"

namespace Engine {
namespace vk {

    glm::mat4 Node::getLocalMatrix() {
        // Since the TRS properties and node matrix are mutually exclusive, we can multiply everything together as 
        // the node matrix or the TRS properties will be its identity value and will have no effect on final transform.
        if (this->dirty)
            this->cachedLocalMatrix = glm::translate(glm::mat4(1.0f), translation) * glm::mat4(rotation) * glm::scale(glm::mat4(1.0f), scale) * this->nodeMatrix;

        return this->cachedLocalMatrix;
    }

    glm::mat4 Node::getModelMatrix() {
        if (!this->dirty)
            return this->cachedMatrix;

        // Need to traverse the node hierarchy to calculate this node's global transformation matrix.
        glm::mat4 matrix = this->getLocalMatrix();

        Node* parent = this->parent;
        while (parent) {
            matrix = parent->getLocalMatrix() * matrix;
            parent = parent->parent;
        }

        // Set the cached matrix and set node as clean
        this->cachedMatrix = matrix;
        this->dirty = false;

        return matrix;
    }

    glm::mat4 Node::getGlobalMatrix() {
        return this->globalMatrix;
    }

	void Node::update() {
        this->dirty = true;

        if (this->mesh) {
            glm::mat4 matrix = this->getModelMatrix();
            if (this->skin) {
                glm::mat4 inverseMatrix = glm::inverse(matrix);

                std::size_t numJoints = std::min((std::uint32_t)this->skin->joints.size(), MAX_JOINTS);
                for (std::size_t j = 0; j < numJoints; j++) {
                    vk::Node* jointNode = this->skin->joints[j];
                    glm::mat4 jointMatrix = jointNode->getModelMatrix() * this->skin->inverseBindMatrices[j];
                    this->skinUniform.jointMatrix[j] = inverseMatrix * jointMatrix;
                    this->skinUniform.isSkinned = 1;
                }
            }
        }
	}

}
}
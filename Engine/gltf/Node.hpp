#pragma once

#include <glm/mat4x4.hpp>
#include <glm/gtc/quaternion.hpp>

#include "Uniforms.hpp"
#include "../vulkan/objects/Buffer.hpp"

namespace Engine {
namespace vk {

	struct Mesh;
	struct Skin;

	struct Node {
		Node* parent;
		Mesh* mesh;
		Skin* skin;
		std::uint32_t skinIndex = -1;
		std::uint32_t index;
		std::vector<Node*> children;

		glm::mat4 nodeMatrix;
		glm::vec3 translation;
		glm::quat rotation;
		glm::vec3 scale{ 1.0f }; // Always make sure scale is default initialised to 1

		glm::mat4 cachedMatrix;
		glm::mat4 cachedLocalMatrix;
		// Marked true when cached matrix should be recalculated
		bool dirty = true;

		// This gets the matrix transformation local to this node.
		// You most likely will want the getModelMatrix() method
		// which returns the global transformation matrix for this node.
		glm::mat4 getLocalMatrix();
		// Returns global transformation matrix for this node.
		glm::mat4 getModelMatrix();

		void update();

		// Bounding box minimums and maximums of this node
		glm::vec3 bbMin;
		glm::vec3 bbMax;

		glsl::SkinningUniform skinUniform;
		Buffer descriptorBuffer;
		VkDescriptorSet descriptor;
	};

}
}
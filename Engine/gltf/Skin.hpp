#pragma once

#include <string>
#include <vector>

#include <glm/mat4x4.hpp>

namespace Engine {
namespace vk {

	struct Node;

	struct Skin {
		std::string name;
		Node* rootNode;
		std::vector<glm::mat4> inverseBindMatrices;
		std::vector<Node*> joints;
	};

}
}
#pragma once

#include <string>
#include <vector>

#include <glm/vec4.hpp>

namespace Engine {
namespace vk {

	struct Node;

	struct AnimationChannel {
		enum PathType {
			TRANSLATION,
			ROTATION,
			SCALE
		};

		PathType pathType;
		Node* node;
		int samplerIndex;
	};

	struct AnimationSampler {
		enum InterpolationType {
			LINEAR,
			STEP,
			CUBIC_SPLINE
		};

		InterpolationType interpolationType;
		std::vector<float> keyframeTimes;
		std::vector<glm::vec4> outputValues;

		void translate(Node* node, float interpolation, std::size_t index);
		void rotate(Node* node, float interpolation, std::size_t index);
		void scale(Node* node, float interpolation, std::size_t index);
	};

	struct Animation {
		std::string name;
		std::vector<AnimationChannel> channels;
		std::vector<AnimationSampler> samplers;

		float start = std::numeric_limits<float>::max();
		float end = std::numeric_limits<float>::min();
	};

}
}
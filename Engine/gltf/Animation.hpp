#pragma once

#include <string>
#include <vector>

#include <glm/vec4.hpp>

namespace Engine {
namespace vk {

	struct Node;
	struct Model;

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

		void translate(Node* node, float interpolation, std::size_t index, bool blend = false);
		void rotate(Node* node, float interpolation, std::size_t index, bool blend = false);
		void scale(Node* node, float interpolation, std::size_t index, bool blend = false);
	};

	struct Animation {
		std::string name;
		std::vector<AnimationChannel> channels;
		std::vector<AnimationSampler> samplers;

		float start = std::numeric_limits<float>::max();
		float end = std::numeric_limits<float>::min();

		bool animating = false;
		// Flag to set whether this animation should loop indefinitely
		bool loop = false;
		float timer;

		void update(Model& model, float timeDelta);
	};

}
}
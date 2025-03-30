#include "Animation.hpp"

#include "glTF.hpp"

namespace Engine {
namespace vk {

	void AnimationSampler::translate(Node* node, float interpolation, std::size_t index) {
		switch (this->interpolationType) {
		case AnimationSampler::InterpolationType::LINEAR: {
			node->translation = glm::mix(outputValues[index], outputValues[index + 1], interpolation);
			break;
		}
		case AnimationSampler::InterpolationType::STEP: {
			node->translation = outputValues[index];
			break;
		}
		case AnimationSampler::InterpolationType::CUBIC_SPLINE: {
			std::cout << "Warning: There is no support for cubic spline interpolation!" << std::endl;
			break;
		}
		}
	}
	
	void AnimationSampler::rotate(Node* node, float interpolation, std::size_t index) {
		switch (this->interpolationType) {
		case AnimationSampler::InterpolationType::LINEAR: {
			glm::quat q1;
			q1.x = outputValues[index].x;
			q1.y = outputValues[index].y;
			q1.z = outputValues[index].z;
			q1.w = outputValues[index].w;

			glm::quat q2;
			q2.x = outputValues[index + 1].x;
			q2.y = outputValues[index + 1].y;
			q2.z = outputValues[index + 1].z;
			q2.w = outputValues[index + 1].w;

			node->rotation = glm::normalize(glm::slerp(q1, q2, interpolation));
			break;
		}
		case AnimationSampler::InterpolationType::STEP: {
			glm::quat q1;
			q1.x = outputValues[index].x;
			q1.y = outputValues[index].y;
			q1.z = outputValues[index].z;
			q1.w = outputValues[index].w;
			
			node->rotation = q1;
			break;
		}
		case AnimationSampler::InterpolationType::CUBIC_SPLINE: {
			std::cout << "Warning: There is no support for cubic spline interpolation!" << std::endl;
			break;
		}
		}
	}
	
	void AnimationSampler::scale(Node* node, float interpolation, std::size_t index) {
		switch (this->interpolationType) {
		case AnimationSampler::InterpolationType::LINEAR: {
			node->scale = glm::mix(outputValues[index], outputValues[index + 1], interpolation);
			break;
		}
		case AnimationSampler::InterpolationType::STEP: {
			node->scale = outputValues[index];
			break;
		}
		case AnimationSampler::InterpolationType::CUBIC_SPLINE: {
			std::cout << "Warning: There is no support for cubic spline interpolation!" << std::endl;
			break;
		}
		}
	}

}
}
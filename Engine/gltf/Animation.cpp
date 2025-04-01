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

	void Animation::update(Model& model, float timeDelta) {
		// Check if we need to stop this animation, unless its a looping animation
		if (this->timer > this->end) {
			this->timer = 0.0f;
			if (!this->loop)
				this->animating = false;
			return;
		}

		bool updated = false;

		for (vk::AnimationChannel& channel : this->channels) {
			vk::AnimationSampler& sampler = this->samplers[channel.samplerIndex];

			// Iterate through the keyframe times for this sampler
			for (std::size_t j = 0; j < sampler.keyframeTimes.size() - 1; j++) {
				float firstKeyframe = sampler.keyframeTimes[j];
				float secondKeyframe = sampler.keyframeTimes[j + 1];

				// Check if the current animation time lies in between a keyframe time and the next keyframe
				if ((this->timer < firstKeyframe) || (this->timer > secondKeyframe))
					continue;

				// Get the interpolation value (how far through) for this keyframe
				float interp = std::max(0.0f, this->timer - firstKeyframe) / (secondKeyframe - firstKeyframe);

				// Apply transformation
				switch (channel.pathType) {
				case vk::AnimationChannel::PathType::TRANSLATION:
					sampler.translate(channel.node, interp, j);
					break;
				case vk::AnimationChannel::PathType::ROTATION:
					sampler.rotate(channel.node, interp, j);
					break;
				case vk::AnimationChannel::PathType::SCALE:
					sampler.scale(channel.node, interp, j);
					break;
				}

				updated = true;
			}
		}

		if (updated) {
			// Update joint matrices of model
			for (Node* node : model.linearNodes)
				node->update();
		}

		this->timer += timeDelta;
	}
}
}
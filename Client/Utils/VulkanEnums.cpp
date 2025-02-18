#include "VulkanEnums.hpp"

#include <cstdio>

namespace Utils {

	VkFilter getVkFilter(int filter) {
		switch (filter) {
		case 9728:
		case 9984:
		case 9985:
			return VK_FILTER_NEAREST;
		case 9729:
		case 9986:
		case 9987:
			return VK_FILTER_LINEAR;
		}

		std::fprintf(stderr, "Unknown filter value %d. Setting to VK_FILTER_NEAREST.\n", filter);
		return VK_FILTER_NEAREST;
	}

	VkSamplerAddressMode getVkSamplerAddressMode(int samplerAddressMode) {
		switch (samplerAddressMode) {
		case 33071:
			return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
		case 33648:
			return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
		case 10497:
			return VK_SAMPLER_ADDRESS_MODE_REPEAT;
		}

		std::fprintf(stderr, "Unknown sampler address mode %d. Setting to VK_SAMPLER_ADDRESS_MODE_REPEAT.\n", samplerAddressMode);
		return VK_SAMPLER_ADDRESS_MODE_REPEAT;
	}
}
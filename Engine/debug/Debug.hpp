#pragma once
#include "../vulkan/VulkanContext.hpp"
#include "../vulkan/VulkanDevice.hpp"

namespace Engine
{
	namespace Debug {

		class DebugManager
		{
		public:
			bool initImGui(VulkanWindow aWindow);
		};

	}
}


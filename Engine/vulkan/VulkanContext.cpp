#include "VulkanContext.hpp"

namespace Engine {

	GLFWwindow* VulkanContext::getGLFWWindow() {
		return window.get()->window;
	}

}
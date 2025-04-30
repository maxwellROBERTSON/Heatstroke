#pragma once

#include <vector>
#include <string>

#include "../vulkan/objects/Buffer.hpp"

#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <volk/volk.h>

namespace Engine {
	struct VulkanContext;
}

enum class Color {
	RED,
	GREEN,
	BLUE,
	ORANGE,
	YELLOW,
	AQUA,
	PINK,
	WHITE,
	BLACK
};

// Crosshair class to define how the crosshair should be rendered.
// Put in the Game side since this is specific to a FPS.
class Crosshair {
public:
	Crosshair() = default;
	Crosshair(Engine::VulkanContext* context);

	void drawCrosshair(VkCommandBuffer cmdBuf);
	void updatePositions();

	bool shouldUpdateCrosshair = true;

	std::vector<const char*> colorNames = {
		"Red", "Green", "Blue", "Orange", "Yellow", "Aqua", "Pink", "White", "Black"
	};

	std::vector<glm::vec4> colorValues = {
		glm::vec4(1.0f, 0.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 0.4f, 0.0f, 1.0f),
		glm::vec4(1.0f, 1.0f, 0.0f, 1.0f),
		glm::vec4(0.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(1.0f, 0.0f, 0.5f, 1.0f),
		glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
		glm::vec4(0.0f, 0.0f, 0.0f, 1.0f)
	};
	int selectedColor = 0;
	
private:
	Engine::VulkanContext* context;

	std::vector<glm::vec2> positions;
	std::vector<glm::vec4> colors;
	std::vector<std::uint32_t> indices;

	Engine::vk::Buffer posBuffer;
	Engine::vk::Buffer colBuffer;
	Engine::vk::Buffer indicesBuffer;

	std::uint32_t vertexCount = 8;
	std::uint32_t indexCount = 12;
};
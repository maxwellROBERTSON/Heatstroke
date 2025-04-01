#pragma once

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>

#include "../Events/Event.hpp"

struct GLFWwindow;

// We put the camera implementation on the game side but do we want to have 
// the camera implementation determined by the game or engine?

namespace Engine
{
	class Camera {
	public:
		Camera() = default;
		Camera(float fov, float near, float far, glm::vec3 position, glm::vec3 frontDirection);
		~Camera() = default;

		void operator=(const Camera& other)
		{
			this->fov = other.fov;
			this->nearPlane = other.nearPlane;
			this->farPlane = other.farPlane;
			this->position = other.position;
			this->frontDirection = other.frontDirection;

			this->firstClick = other.firstClick;

			this->yaw = other.yaw;
			this->pitch = other.pitch;
			this->lastX = other.lastX;
			this->lastY = other.lastY;
		}

		void updateCamera(GLFWwindow* aWindow, float timeDelta);
		void OnEvent(GLFWwindow* aWindow, Engine::Event& e);

		float fov;
		float nearPlane;
		float farPlane;
		glm::vec3 position;
		glm::vec3 frontDirection;

		bool firstClick = true;

		float yaw = 0.0f;
		float pitch = 0.0f;
		float lastX = 0.0f;
		float lastY = 0.0f;

	};
}

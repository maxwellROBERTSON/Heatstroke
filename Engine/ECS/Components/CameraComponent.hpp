#pragma once

#include <string>

#include "../Core/Camera.hpp"

namespace Engine
{
	class Camera;
}

#include "Component.hpp"

namespace Engine
{
	class CameraComponent : public Component<CameraComponent>
	{
	public:
		CameraComponent() {};

		void operator=(const CameraComponent& other) override
		{
			this->camera = other.camera;
		}

		// Getters

		// Static type getter from Component parent
		ComponentTypes static StaticType() { return ComponentTypes::CAMERA; }

		// Get component data
		void GetDataArray(uint8_t*) override;

		// Get camera pointer
		Engine::Camera* GetCamera() { return camera; }

		// Setters	

		// Set component data
		void SetDataArray(uint8_t*) override;

		// Set camera pointer
		void SetCamera(Engine::Camera* aCamera) { camera = aCamera; }

	private:
		Engine::Camera* camera;
	};
}

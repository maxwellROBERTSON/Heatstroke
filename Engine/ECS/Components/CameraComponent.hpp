#pragma once

#include <string>

#include "../Core/Camera.hpp"
#include "../EntityManager.hpp"

namespace Engine
{
	class Camera;
	class EntityManager;
	class Entity;
}

#include "Component.hpp"

namespace Engine
{
	class CameraComponent : public Component<CameraComponent>
	{
	public:
		CameraComponent() : entityManager(nullptr), entity(nullptr) {}
		CameraComponent(Engine::Camera c) : entityManager(nullptr), entity(nullptr), camera(c) {}
		CameraComponent(Engine::EntityManager* entityManager, Engine::Entity* entity) : entityManager(entityManager), entity(entity) {}

		void operator=(const CameraComponent& other) override
		{
			this->camera = other.camera;
		}

		// Getters

		// Static type getter from Component parent
		ComponentTypes static StaticType() { return ComponentTypes::CAMERA; }

		// Static size getter from Component parent
		size_t static StaticSize() { return sizeof(camera.fov) + sizeof(camera.nearPlane) + sizeof(camera.farPlane) + sizeof(camera.position) + sizeof(camera.frontDirection); }

		// Get component data
		void GetDataArray(uint8_t*) override;

		// Get camera pointer
		Engine::Camera* GetCamera() { return camera; }

		// Setters	

		// Set component data
		void SetDataArray(uint8_t*) override;

		// Update camera using Camera class
		void UpdateCamera(GLFWwindow*, float);

		void SetCamera(Engine::Camera aCamera) { camera = aCamera; SetComponentHasChanged(); }

		// Set component has changed in entity manager
		void SetComponentHasChanged();

		// Toggle has changed boolean
		void ToggleHasChanged() { hasChanged = !hasChanged; }

	private:
		// EntityManager pointer
		Engine::EntityManager* entityManager;
		// Entity pointer
		Engine::Entity* entity;

		// If component has changed since last network update
		bool hasChanged = false;

		Engine::Camera camera;
	};
}

	// 	// Set camera pointer
	// 	void SetCamera(Engine::Camera* aCamera) { camera = aCamera; }

	// private:
	// 	Engine::Camera* camera;

#pragma once

#include <string>

#include "../Core/Camera.hpp"

#include "Component.hpp"

#include "../EntityManager.hpp"

namespace Engine
{
	class Camera;
	class EntityManager;
	class Entity;

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

		// Type getter from Component parent
		ComponentTypes StaticType() const override { return ComponentTypes::CAMERA; }

		// Size getter from Component parent
		size_t StaticSize() const override { return sizeof(camera.fov) + sizeof(camera.nearPlane) + sizeof(camera.farPlane) + sizeof(camera.position) + sizeof(camera.frontDirection); }

		// Get component data
		void GetDataArray(uint8_t*) override;

		// Get camera pointer
		Engine::Camera* GetCamera() { return &camera; }

		// Setters

		// Set component data
		void SetDataArray(uint8_t*) override;

		// Update camera using Camera class
		void UpdateCamera(GLFWwindow*, float);

		// Set camera pointer
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

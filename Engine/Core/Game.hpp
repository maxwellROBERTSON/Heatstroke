#pragma once

#include <memory>

#include "RenderMode.hpp"
#include "../Events/Event.hpp"
#include "Camera.hpp"
#include "../Events/KeyEvent.hpp"
#include "../Events/MouseEvent.hpp"
#include "../Events/WindowEvent.hpp"
#include "../Input/Callbacks.hpp"
#include "../Input/Input.hpp"
#include "../Input/InputCodes.hpp"
#include "../Network/Client/GameClient.hpp"
#include "../vulkan/VulkanContext.hpp"
#include "../Physics/PhysicsWorld.hpp"
#include "../vulkan/VulkanContext.hpp"
#include "../vulkan/Renderer.hpp"
#include "../GUI/GUI.hpp"
#include "../ECS/ComponentTypeRegistry.hpp"
#include "../ECS/EntityManager.hpp"

namespace Engine
{
	class Camera;
	class Renderer;
	class GUI;

	class Game
	{
	public:
		Game(const std::string& name = "Heatstroke", int width = 1280, int height = 720);
		virtual ~Game() = default;
		virtual void Init() {}
		virtual void Update() {}
		virtual void Run();
		virtual void OnEvent(Event& e);

		virtual void loadOfflineEntities() {}

		void ResetRenderModes();

		// Getters
		inline VulkanContext& GetContext() { return mContext; }
		inline static Game& Get() { return *game; }
		inline std::vector<Engine::vk::Model>& GetModels() { return models; }
		inline bool GetRecreateSwapchain() { return recreateSwapchain; }
		inline ComponentTypeRegistry& GetRegistry() { return registry; }
		inline EntityManager& GetEntityManager() { return entityManager; }
		inline Engine::Renderer& GetRenderer() { return *renderer; }
		inline PhysicsWorld& GetPhysicsWorld() { return physics_world; }
		inline Engine::GUI& GetGUI() { return *gui; }
		inline unsigned int* GetRenderModes() { return &renderModes; }
		inline bool GetRenderMode(Engine::RenderMode r) { return (renderModes & (1 << r)) != 0; }
		Engine::RenderMode GetGUIRenderMode();

		// Setters
		inline void ToggleRenderMode(Engine::RenderMode r) { renderModes ^= (1 << r); }

		bool OnWindowClose(WindowCloseEvent& e);
	private:
		VulkanContext mContext;
		static Game* game;
		std::vector<Engine::vk::Model> models;
		bool recreateSwapchain;
		ComponentTypeRegistry registry = ComponentTypeRegistry::Get();
		EntityManager entityManager = EntityManager(&registry);
		std::unique_ptr<Engine::Renderer> renderer;
		PhysicsWorld physics_world;
		std::unique_ptr<Engine::GUI> gui;
		unsigned int renderModes;
		//std::vector<int> renderModes = std::vector<int>(Engine::RenderMode::COUNT);

		bool isRunning = true;
		float deltaTime = 0.0f, lastTime = 0.0f;

	};
}


#pragma once

#include "../Events/Event.hpp"

#include <memory>

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
#include "../GUI/GUI.hpp"
#include "../vulkan/Renderer.hpp"
#include "../ECS/ComponentTypeRegistry.hpp"
#include "../ECS/EntityManager.hpp"

namespace Engine
{
	class GUI;
	class Renderer;

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
		inline Engine::RenderMode& GetRenderMode() { return renderMode; }

		bool OnWindowClose(WindowCloseEvent& e);
	private:
		bool isRunning = true;
		VulkanContext mContext;
		static Game* game;
		std::vector<Engine::vk::Model> models;
		bool recreateSwapchain;
		ComponentTypeRegistry registry = ComponentTypeRegistry::Get();
		EntityManager entityManager = EntityManager(&registry);
		std::unique_ptr<Engine::Renderer> renderer;
		PhysicsWorld physics_world;
		std::unique_ptr<Engine::GUI> gui;
		Engine::RenderMode renderMode = GUIX;

		//Game(const std::string& name, int width, int height)
		//	: mContext(initialiseVulkan(name, width, height)),
		//	registry(ComponentTypeRegistry::Get()),
		//	entityManager(&registry),
		//	physics_world(),
		//	GUI(this),
		//	renderer(&this->GetContext(), &this->GetEntityManager())  // Initialize renderer with context and entity manager
		//{
		//	// Your initialization code here if needed
		//}

		float deltaTime = 0.0f, lastTime = 0.0f;

	};
}


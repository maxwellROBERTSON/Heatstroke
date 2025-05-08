#pragma once

#include <memory>

#include "../ECS/EntityManager.hpp"
#include "../Events/Event.hpp"
#include "../Events/KeyEvent.hpp"
#include "../Events/MouseEvent.hpp"
#include "../Events/WindowEvent.hpp"
#include "../Input/Callbacks.hpp"
#include "../Input/Input.hpp"
#include "../Input/InputCodes.hpp"
#include "../Network/Network.hpp"
#include "../Physics/PhysicsWorld.hpp"
#include "../vulkan/VulkanContext.hpp"
#include "../GUI/GUI.hpp"
#include "../ThreadPool/thread_pool_wait.h"

#include "Camera.hpp"


namespace Engine
{
	class Camera;
	class GUI;
	class Network;

	class Game
	{
	public:
		Game(const std::string& name = "Heatstroke", int width = 1280, int height = 720);
		virtual ~Game() = default;
		virtual void Init() {}
		virtual void Render() {}
		virtual void Update() {}
		virtual void Run();
		virtual void OnEvent(Event& e);

		// Getters
		inline VulkanContext& GetContext() { return mContext; }
		inline static Game& Get() { return *game; }
		inline std::vector<Engine::vk::Model>& GetModels() { return models; }
		inline bool GetRecreateSwapchain() { return recreateSwapchain; }
		inline EntityManager& GetEntityManager() { return entityManager; }
		inline PhysicsWorld& GetPhysicsWorld() { return physics_world; }
		inline Engine::Network& GetNetwork() { return *network; }
		inline Engine::GUI& GetGUI() { return *gui; }

		// Setters
		void SetClient(yojimbo::Address);
		void SetServer(uint16_t, int);

		bool OnWindowClose(WindowCloseEvent& e);
		static int instanceCount;

	private:
		VulkanContext mContext;
		inline static Game* game;
		std::vector<Engine::vk::Model> models;
		bool recreateSwapchain;
		EntityManager entityManager = EntityManager();
		PhysicsWorld physics_world;
		std::unique_ptr<Engine::Network> network;
		std::unique_ptr<Engine::GUI> gui;
		thread_pool_wait* threadPool;

		float deltaTime = 0.0f, lastTime = 0.0f;
	};
}

#pragma once

#include <memory>

#include "../ECS/EntityManager.hpp"
#include "../Events/Event.hpp"
#include "../Events/KeyEvent.hpp"
#include "../Events/MouseEvent.hpp"
#include "../Events/RenderModeEvent.hpp"
#include "../Events/WindowEvent.hpp"
#include "../GUI/GUI.hpp"
#include "../Input/Callbacks.hpp"
#include "../Input/Input.hpp"
#include "../Input/InputCodes.hpp"
#include "../Network/Network.hpp"
#include "../Physics/PhysicsWorld.hpp"
#include "../vulkan/Renderer.hpp"
#include "../vulkan/VulkanContext.hpp"

#include "Camera.hpp"
#include "RenderMode.hpp"

namespace Engine
{
	class Camera;
	class Renderer;
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
		virtual void DrawGUI() {}
		virtual void DrawDebugGUI() {}
		virtual void OnEvent(Event& e);

		virtual void loadOfflineEntities() {}
		virtual void loadOnlineEntities(int) {}

		void ResetRenderModes();

		// Getters
		inline VulkanContext& GetContext() { return mContext; }
		inline static Game& Get() { return *game; }
		inline std::vector<Engine::vk::Model>& GetModels() { return models; }
		inline bool GetRecreateSwapchain() { return recreateSwapchain; }
		inline EntityManager& GetEntityManager() { return entityManager; }
		inline Engine::Renderer& GetRenderer() { return *renderer; }
		inline PhysicsWorld& GetPhysicsWorld() { return physics_world; }
		inline Engine::GUI& GetGUI() { return *gui; }
		inline unsigned int* GetRenderModes() { return &renderModes; }
		inline bool GetRenderMode(Engine::RenderMode r) { return (renderModes & (1 << r)) != 0; }
		Engine::RenderMode GetGUIRenderMode();
		inline Engine::Network& GetNetwork() { return *network; }


		// Setters
		inline void ToggleRenderMode(Engine::RenderMode r) {
			renderModes ^= (1 << r);
			/*RenderModeToggleEvent event(r, on);
				VulkanWindow& engineWindow = *mContext.window.get();
				engineWindow.EventCallback(event);*/
		}
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
		std::unique_ptr<Engine::Renderer> renderer;
		std::unique_ptr<Engine::GUI> gui;
		std::unique_ptr<Engine::Network> network;
		unsigned int renderModes;

		float deltaTime = 0.0f, lastTime = 0.0f;
	};
}

#pragma once

#include "../Events/Event.hpp"
#include "../Events/KeyEvent.hpp"
#include "../Events/MouseEvent.hpp"
#include "../Events/WindowEvent.hpp"
#include "../Input/Callbacks.hpp"
#include "../Input/Input.hpp"
#include "../Input/InputCodes.hpp"
#include "../Network/Client/GameClient.hpp"
#include "../vulkan/VulkanContext.hpp"

namespace Engine
{
	class Game
	{
	public:
		Game(const std::string& name = "Heatstroke", int width = 1280, int height = 720);
		virtual ~Game() = default;
		virtual void Init() {}
		virtual void Update() {}
		virtual void Run();
		virtual void OnEvent(Event& e);
		inline VulkanContext& GetContext() { return mContext; }
		inline static Game& Get() { return *game; }
		bool OnWindowClose(WindowCloseEvent& e);
	private:
		bool isRunning = true;
		VulkanContext mContext;
		static Game* game;
		float deltaTime = 0.0f, lastTime = 0.0f;

	};
}


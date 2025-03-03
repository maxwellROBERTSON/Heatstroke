#pragma once

#include "../Events/Event.h"
#include "../Events/KeyEvent.h"
#include "../Events/WindowEvent.h"
#include "../Input/Callbacks.hpp"
#include "../Input/InputCodes.h"
#include "../Network/Client/GameClient.hpp"
#include "../vulkan/VulkanContext.hpp"

namespace Engine
{
	class Game
	{
	public:
		Game(const std::string& name = "Heatstroke", int width = 1280, int height = 720);
		virtual ~Game() = default;
		virtual void Init() {
			std::cout << "ENGINE INIT" << std::endl;
		}
		virtual void Update() {}
		void Run();
		virtual void OnEvent(Event& e);
		inline VulkanContext& GetContext() { return mContext; }
		inline static Game& Get() { return *game; }

	private:
		bool isRunning = true;
		VulkanContext mContext;
		static Game* game;

		float deltaTime = 0.0f, lastTime = 0.0f;

		bool OnWindowClose(WindowCloseEvent& e);
	};
}


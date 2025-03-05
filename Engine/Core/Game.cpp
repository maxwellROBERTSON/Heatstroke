#include "Game.h"

namespace Engine
{
	Game::Game(const std::string& name, int width, int height)
	{
		mContext.window = initialiseVulkan(name, width, height);
		mContext.allocator = createVulkanAllocator(*mContext.window.get());
		registerCallbacks(this->GetContext().getGLFWWindow());
		this->Init();
	}
	void Game::Run()
	{
		this->Update();
	}

	void Game::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Game::OnWindowClose, this, std::placeholders::_1));

		dispatcher.Dispatch<KeyPressedEvent>(
			[&](KeyPressedEvent& event)
			{
				if (event.GetKeyCode() == HS_KEY_ESCAPE)
				{
					WindowCloseEvent close;
					OnWindowClose(close);
					return true;
				}
			}
		);

	}
	bool Game::OnWindowClose(WindowCloseEvent& e)
	{
		isRunning = false;
		return true;
	}
}
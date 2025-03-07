#include "Game.h"

namespace Engine
{
	Game::Game(const std::string& name, int width, int height)
	{
		mContext.window = initialiseVulkan(name, width, height);
		mContext.allocator = createVulkanAllocator(*mContext.window.get());
		mContext.window.get()->SetEventCallback(std::bind(&Game::OnEvent, this, std::placeholders::_1));
		this->Init();
	}
	//void Game::Run()
	//{
	//	//while (isRunning)
	//	//{
	//	//	//float currTime = (float)glfwGetTime();
	//	//	//deltaTime = currTime - lastTime;
	//	//	//lastTime = currTime;
	//	//	//this->Update();
	//	//	Update();
	//	//}
	//}

	void Game::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Game::OnWindowClose, this, std::placeholders::_1));


		dispatcher.Dispatch<KeyPressedEvent>(
			[&](KeyPressedEvent& event)
			{
				if (event.GetKeyCode() == HS_KEY_ESCAPE)
				{
					std::cout << "Game::Event" << std::endl;
					WindowCloseEvent close;
					OnWindowClose(close);
					return true;
				}
			}
		);


		dispatcher.Dispatch<MouseButtonPressedEvent>(
			[&](MouseButtonPressedEvent& event)
			{
				std::cout << event.GetMouseButton() << std::endl;
				return true;
			}
		);

	}
	bool Game::OnWindowClose(WindowCloseEvent& e)
	{
		isRunning = false;
		return true;
	}
}
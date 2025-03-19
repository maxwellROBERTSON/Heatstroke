#include "Game.hpp"

namespace Engine
{
	Game::Game(const std::string& name, int width, int height)
	{
		mContext.window = initialiseVulkan(name, width, height);
		mContext.allocator = createVulkanAllocator(*mContext.window.get());
		//sInputManager->registerCallbacks(mContext.window.get());
		InputManager::RegisterCallbacks(mContext.window.get());
		mContext.window.get()->SetEventCallback(std::bind(&Game::OnEvent, this, std::placeholders::_1));
		//this->Init();
		//registerCallbacks(this->GetContext().getGLFWWindow(), this);
		renderer = std::make_unique<Renderer>(&this->GetContext(), &this->entityManager);
		gui = std::make_unique<GUI>(this);
		this->Init();
	}
	void Game::Run()
	{
		//while (isRunning)
		//{
		//	//float currTime = (float)glfwGetTime();
		//	//deltaTime = currTime - lastTime;
		//	//lastTime = currTime;

			this->Update();
		//}
	}

	void Game::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(std::bind(&Game::OnWindowClose, this, std::placeholders::_1));
	}

	bool Game::OnWindowClose(WindowCloseEvent& e)
	{
		glfwSetWindowShouldClose(mContext.getGLFWWindow(), true);
		return true;
	}
}
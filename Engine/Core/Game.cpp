#include "Game.hpp"

#include <GLFW/glfw3.h>

namespace Engine
{
	int Game::instanceCount = 0;

	Game::Game(const std::string& name, int width, int height)
	{
		instanceCount++;
		mContext.window = initialiseVulkan(name, width, height);
		mContext.allocator = createVulkanAllocator(*mContext.window.get());
		InputManager::RegisterCallbacks(mContext.window.get());
		mContext.window.get()->SetEventCallback(std::bind(&Game::OnEvent, this, std::placeholders::_1));
		network = std::make_unique<Network>();
		gui = std::make_unique<GUI>(this);
		threadPool = thread_pool_wait::get_instance();
		this->Init();
	}
	void Game::Run()
	{
		this->Render();
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

	void Game::SetClient(yojimbo::Address serverAddress)
	{
		network->InitializeClient(this, serverAddress);
	}

	void Game::SetServer(uint16_t port, int maxClients)
	{
		network->InitializeServer(this, port, maxClients);
	}

}
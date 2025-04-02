#include "Game.hpp"

namespace Engine
{
	Game::Game(const std::string& name, int width, int height)
	{
		mContext.window = initialiseVulkan(name, width, height);
		mContext.allocator = createVulkanAllocator(*mContext.window.get());
		InputManager::RegisterCallbacks(mContext.window.get());
		mContext.window.get()->SetEventCallback(std::bind(&Game::OnEvent, this, std::placeholders::_1));
		renderer = std::make_unique<Renderer>(&this->GetContext(), &this->entityManager, this);
		gui = std::make_unique<GUI>(this);
		network = std::make_unique<Network>();
		ResetRenderModes();
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
		dispatcher.Dispatch<ESCEvent>([this](Event& event) { gui->toggle(); return true; });
		/*dispatcher.Dispatch<RenderModeToggleEvent>([this](RenderModeToggleEvent& event)
			{
				if (event.IsOn())
					renderer->modeOn(event.GetRenderMode());
				else renderer->modeOff(event.GetRenderMode());
				return true;
			});*/
	}

	bool Game::OnWindowClose(WindowCloseEvent& e)
	{
		glfwSetWindowShouldClose(mContext.getGLFWWindow(), true);
		return true;
	}

	void Game::ResetRenderModes()
	{
#ifdef _DEBUG
		renderModes = 0b0000011;
#else
		renderModes = 0b0000010
#endif
	}

	Engine::RenderMode Game::GetGUIRenderMode()
	{
		if ((renderModes & (1 << GUIHOME)))
			return GUIHOME;
		else if ((renderModes & (1 << GUISETTINGS)))
			return GUISETTINGS;
		else if ((renderModes & (1 << GUILOADING)))
			return GUILOADING;
		else if ((renderModes & (1 << GUISERVER)))
			return GUISERVER;
		else if ((renderModes & (1 << GUIDEBUG)))
			return GUIDEBUG;
		else
			return COUNT;
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
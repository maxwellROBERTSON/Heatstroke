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
		renderer = std::make_unique<Renderer>(&this->GetContext(), &this->entityManager, this);
		gui = std::make_unique<GUI>(this);
		ResetRenderModes();
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
		dispatcher.Dispatch<ESCEvent>([this](Event& event) { gui->toggle(); return true; });
		dispatcher.Dispatch<RenderModeToggleEvent>([this](RenderModeToggleEvent& event)
			{
				if (event.IsOn())
					renderer->modeOn(event.GetRenderMode());
				else renderer->modeOff(event.GetRenderMode());
				return true;
			});
	}

	bool Game::OnWindowClose(WindowCloseEvent& e)
	{
		glfwSetWindowShouldClose(mContext.getGLFWWindow(), true);
		return true;
	}

	void Game::ResetRenderModes()
	{
#ifdef _DEBUG
		renderModes = 0b0100011;
#else
		renderModes = 0b0100010
#endif
		static const std::map<int, std::string> modeNames = {
		{ GUIDEBUG, "GUIDEBUG" },
		{ GUIHOME, "GUIHOME" },
		{ GUISETTINGS, "GUISETTINGS" },
		{ FORWARD, "FORWARD" },
		{ DEFERRED, "DEFERRED" },
		{ SHADOWS, "SHADOWS" },
		{ COUNT, "COUNT" }
		};
		std::cout << "INIT:" << std::endl;
		for (const auto& [mode, name] : modeNames)
		{
			std::cout << name << " = " << (((renderModes >> mode) & 1) ? "ON" : "OFF") << " ";
		}
		/*std::cout << ((renderModes >> 0) & 1) << std::endl;
		std::cout << ((renderModes >> 1) & 1) << std::endl;
		std::cout << ((renderModes >> 2) & 1) << std::endl;
		std::cout << ((renderModes >> 3) & 1) << std::endl;
		std::cout << ((renderModes >> 4) & 1) << std::endl;
		std::cout << ((renderModes >> 5) & 1) << std::endl;
		std::cout << ((renderModes >> 6) & 1) << std::endl;
		std::cout << ((renderModes >> 7) & 1) << std::endl;*/
		//std::cout << "\n" << renderModes << std::endl;
		std::cout << "\n" << std::endl;
	}

	Engine::RenderMode Game::GetGUIRenderMode()
	{
		if ((renderModes & (1 << GUIHOME)))
			return GUIHOME;
		else if ((renderModes & (1 << GUISETTINGS)))
			return GUISETTINGS;
		else if ((renderModes & (1 << GUIDEBUG)))
			return GUIDEBUG;
		else
			return COUNT;
	}
}
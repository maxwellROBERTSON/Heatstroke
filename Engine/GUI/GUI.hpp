#pragma once

#include <functional>
#include <string>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>
//#include "../third_party/imgui/misc/fonts/IconFontCppHeaders/IconsFontAwesome5.h"

#include "../vulkan/VulkanContext.hpp"
#include "../vulkan/VulkanDevice.hpp"
#include "../Input/Keyboard.hpp"
#include "../Core/Game.hpp"

#include "Error.hpp"
#include "toString.hpp"

struct GLFWwindow;

namespace Engine
{
	class Game;
}

namespace Engine
{
	class GUI
	{
	public:
		// Constructors
		GUI() {}
		GUI(Engine::Game* game) : game(game)
		{
			AddFunction(GUIDEBUG, [this](int* w, int* h) { makeDebugGUI(w, h); });
			AddFunction(GUIHOME, [this](int* w, int* h) { makeHomeGUI(w, h); });
			AddFunction(GUISETTINGS, [this](int* w, int* h) { makeSettingsGUI(w, h); });
			AddFunction(GUISERVER, [this](int* w, int* h) { makeServerGUI(w, h); });
			AddFunction(GUILOADING, [this](int* w, int* h) { makeLoadingGUI(w, h); });
		}

		void initGUI();

		void toggle();

		void AddFunction(Engine::RenderMode r, std::function<void(int*, int*)> func) { functions.emplace(r, func); }

		void makeGUI();
		void makeHomeGUI(int*, int*);
		void makeSettingsGUI(int*, int*);
		void makeDebugGUI(int*, int*);
		void ShowInputDebug();
		void makeServerGUI(int*, int*);
		void makeLoadingGUI(int*, int*);

		// from input actions
		bool debugInput{ false };
		bool debugGame{ true };
		// from input actions

		ImGui_ImplVulkanH_Window imGuiWindow;
		bool changedMSAA = false;

		ImFont* defaultFont;
		ImFont* gameFont;

	private:
		std::map<Engine::RenderMode, std::function<void(int*, int*)>> functions;
		Engine::Game* game;

		// Pop-up info
		bool multiplayerSelected = false;
		bool serverSelected = false;
		std::string errorMsg = "";
		ImVec2 serverBoxSize = ImVec2(0, 0);

		// Loading info
		std::string loadingMsg = "Messages not yet setup. Need to put this onto a thread.";
	};
}
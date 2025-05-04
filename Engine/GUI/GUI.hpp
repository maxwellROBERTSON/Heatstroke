#pragma once

#include <functional>
#include <string>
#include <vector>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
#include <imgui.h>

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
		// Constructor
		GUI(Engine::Game* game) : game(game) {}

		// Getters

		ImFont* GetFont(std::string s);

		bool GetGUIMode(std::string s);

		// Setters

		void initGUI();

		void makeGUI();

		void AddFunction(std::string s, std::function<void(int*, int*)> func);

		void AddFont(std::string s, const char* filename, float size);

		void ToggleGUIMode(std::string s);

	private:
		Engine::Game* game;

		std::vector<std::string> guiModes = std::vector<std::string>(0);
		std::vector<bool> activeGUIModes = std::vector<bool>(0);
		std::map<std::string, std::function<void(int*, int*)>> functions;

		std::map<std::string, ImFont*> fonts;

		bool isNullFrame = true;
	};
}
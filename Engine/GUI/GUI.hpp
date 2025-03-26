#pragma once

//#include <glm/mat4x4.hpp>
//#include <glm/vec4.hpp>
//
//#include "../Events/Event.h"
//#include "../Events/KeyEvent.h"
//#include "../Events/MouseEvent.h"

//#include <map>
#include <string>
//#include <any>
#include <functional>
//#include <typeindex>

#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_vulkan.h>
//#include "../third_party/imgui/misc/fonts/IconFontCppHeaders/IconsFontAwesome5.h"

//#define GLFW_INCLUDE_NONE
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

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
		GUI() {}
		GUI(Engine::Game* game) : game(game)
		{
			AddFunction(GUIDEBUG, [this](int* w, int* h) { makeDebugGUI(w, h); });
			AddFunction(GUIHOME, [this](int* w, int* h) { makeHomeGUI(w, h); });
			AddFunction(GUISETTINGS, [this](int* w, int* h) { makeSettingsGUI(w, h); });
		}

		void initGUI();

		void toggle();

		void AddFunction(Engine::RenderMode r, std::function<void(int*, int*)> func) { functions.emplace(r, func); }

		void makeGUI();
		void makeHomeGUI(int*, int*);
		void makeSettingsGUI(int*, int*);
		void makeDebugGUI(int*, int*);

	private:
		std::map<Engine::RenderMode, std::function<void(int*, int*)>> functions;
		Engine::Game* game;
	};
}
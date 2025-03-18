#include "GUI.hpp"

namespace Engine
{
	void GUI::initGUI()
	{
		//ImGui_ImplVulkan_LoadFunctions(vkGetInstanceProcAddr, vkGetDeviceProcAddr);
		IMGUI_CHECKVERSION();
		ImGuiContext* ImGuiContext = ImGui::CreateContext();
		ImGui::SetCurrentContext(ImGuiContext);
		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
		//ImFontConfig config;
		//config.MergeMode = true;
		//config.GlyphMinAdvanceX = 13.0f; // Use if you want to make the icon monospaced
		//static const ImWchar icon_ranges[] = { ICON_MIN_FA, ICON_MAX_FA, 0 };
		//std::string s = "../Engine/third_party/imgui/misc/fonts/FontAwesome.ttf";
		//io.Fonts->AddFontFromFileTTF(s.c_str(), 13.0f, &config, icon_ranges);

		// Setup Dear ImGui style
		ImGui::StyleColorsDark();
		//ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplGlfw_InitForVulkan(&(*game->GetContext().getGLFWWindow()), true);
		ImGui_ImplVulkan_InitInfo init_info = {};
		Engine::VulkanWindow* window = &(*game->GetContext().window);

		//init_info.ApiVersion = VK_API_VERSION_1_3;              // Pass in your value of VkApplicationInfo::apiVersion, otherwise will default to header version.
		init_info.Instance = window->instance;
		init_info.PhysicalDevice = window->physicalDevice;
		init_info.Device = window->device->device;
		init_info.QueueFamily = window->graphicsFamilyIndex;
		init_info.Queue = window->graphicsQueue;
		init_info.DescriptorPool = window->device->dPool;
		init_info.RenderPass = game->GetRenderer().GetRenderPass("default");
		init_info.Subpass = 0;

		VkSurfaceCapabilitiesKHR caps;
		if (const auto res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(window->physicalDevice, window->surface, &caps); VK_SUCCESS != res)
			throw Utils::Error("Unable to get surface capabilities\n vkGetPhysicalDeviceSurfaceCapabilitiesKHR() returned %s", Utils::toString(res).c_str());

		std::uint32_t imageCount = 2;

		if (imageCount < caps.minImageCount + 1)
			imageCount = caps.minImageCount + 1;

		if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
			imageCount = caps.maxImageCount;

		init_info.MinImageCount = caps.minImageCount;
		init_info.ImageCount = imageCount;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&init_info);
	}

	void GUI::toggle()
	{
		for (const auto& [key, state] : Keyboard::getKeyStates())
		{
			if (key == GLFW_KEY_ESCAPE && !isActive)
			{
				if (state.first && !previousState)
				{
					isActive = true;
					previousState = true;
					break;
				}
				if (state.first && previousState)
				{
					break;
				}
				if (!state.first)
				{
					previousState = false;
				}
			}
			else if (key == GLFW_KEY_ESCAPE && isActive)
			{
				if (!state.first)
				{
					previousState = false;
					break;
				}
				if (state.first && previousState)
				{
					break;
				}
				if (state.first)
				{
					isActive = false;
					previousState = true;
				}
			}
		}
	}

	void GUI::makeGUI()
	{
		int width;
		int height;
		glfwGetFramebufferSize(game->GetContext().getGLFWWindow(), &width, &height);

		functions[activeGUI](&width, &height);

		ImGui::Render();
	}

	void GUI::makeHomeGUI(int* w, int* h)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec4 clear_color = ImVec4(0.8f, 0.5f, 0.5f, 1.00f);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(*w, *h));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));

		ImGui::Begin("Home Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		ImGui::Text("Demo game made using Heatstroke", ImVec2(*w / 4, *h / 4));
		if (ImGui::Button("Single Player", ImVec2(*w / 4, *h / 4)))
		{
			game->loadOfflineEntities();
			game->GetRenderer().initialiseModelMatrices();
			game->GetRenderMode() = FORWARD;
			std::string s = "Settings";
			activeGUI = s;
			/*isActive = false;
			isActive = true;*/
		}
		if (ImGui::Button("Multi-Player", ImVec2(*w / 4, *h / 4)))
		{
			// nothing to be done yet
		}

		ImVec2 windowSize = ImVec2(*w / 4, *h / 4);
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 topRightPos = ImVec2(*w - *w / 6 - 10, 30); // Adjust for button size

		// Set cursor to top-right position
		ImGui::SetCursorPos(topRightPos);
		// ICON_FA_WRENCH
		if (ImGui::Button("Settings", ImVec2(*w / 6, *h / 6)))
		{
			std::string s = "Settings";
			activeGUI = s;
		}

		ImGui::End();

		ImGui::PopStyleColor(3);
	}

	void GUI::makeSettingsGUI(int* w, int* h)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec4 clear_color = ImVec4(0.8f, 0.5f, 0.5f, 1.00f);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(*w, *h));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));

		ImGui::Begin("Settings Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		ImGui::Text("List of settings would go here", ImVec2(*w / 4, *h / 4));
		if (ImGui::Button("Setting 1", ImVec2(*w / 4, *h / 4)))
		{
			// nothing to be done yet
		}
		if (ImGui::Button("Setting 2", ImVec2(*w / 4, *h / 4)))
		{
			// nothing to be done yet
		}

		ImVec2 windowSize = ImVec2(*w / 4, *h / 4);
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 topRightPos = ImVec2(*w - *w / 6 - 10, 30); // Adjust for button size

		// Set cursor to top-right position
		ImGui::SetCursorPos(topRightPos);
		if (ImGui::Button("Home", ImVec2(*w / 6, *h / 6)))
		{
			std::string s = "Home";
			activeGUI = s;
			game->GetRenderMode() = GUIX;

			if (game->GetRenderer().GetIsSceneLoaded())
			{
				game->GetEntityManager().ClearManager();

			}
			//if (isActive)
			//{
			//	isActive = false;
			//	game->GetEntityManager().ClearManager();
			//	//deleteSceneAndModelDescriptors();
			//}
		}

		ImGui::End();

		ImGui::PopStyleColor(3);
	}

	void GUI::makeDebugGUI(int* w, int* h)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec4 clear_color = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::Begin("Debug Menu");

		ImGui::Text("List of info would go here", ImVec2(*w / 4, *h / 4));

		glm::vec3 pos = game->GetRenderer().GetCamera()->position;
		glm::vec3 fDir = game->GetRenderer().GetCamera()->frontDirection;
		std::string posStr = "X: " + std::to_string(pos.x) +
			" Y: " + std::to_string(pos.y) +
			" Z: " + std::to_string(pos.z);

		std::string fDirStr = "X: " + std::to_string(fDir.x) +
			" Y: " + std::to_string(fDir.y) +
			" Z: " + std::to_string(fDir.z);

		ImGui::Text("Position:");
		ImGui::InputText("##Position", (char*)posStr.c_str(), posStr.size() + 1, ImGuiInputTextFlags_ReadOnly);

		ImGui::Text("FrontDir:");
		ImGui::InputText("##FrontDir", (char*)fDirStr.c_str(), fDirStr.size() + 1, ImGuiInputTextFlags_ReadOnly);

		ImGui::End();
	}
}
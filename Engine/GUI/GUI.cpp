#include "GUI.hpp"
#include "GLFW/glfw3.h"

class GLFWwindow;

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
		init_info.RenderPass = game->GetRenderer().GetRenderPass("forward");
		init_info.Subpass = 0;

		VkSurfaceCapabilitiesKHR caps;
		if (const auto res = vkGetPhysicalDeviceSurfaceCapabilitiesKHR(window->physicalDevice, window->surface, &caps); VK_SUCCESS != res)
			throw Utils::Error("Unable to get surface capabilities\n vkGetPhysicalDeviceSurfaceCapabilitiesKHR() returned %s", Utils::toString(res).c_str());

		std::uint32_t imageCount = 2;

		if (imageCount < caps.minImageCount + 1)
			imageCount = caps.minImageCount + 1;

		if (caps.maxImageCount > 0 && imageCount > caps.maxImageCount)
			imageCount = caps.maxImageCount;

		init_info.MinImageCount = caps.minImageCount < 2 ? 2 : caps.minImageCount;
		init_info.ImageCount = imageCount;
		init_info.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

		ImGui_ImplVulkan_Init(&init_info);
	}

	void GUI::toggle()
	{
		Engine::RenderMode mode = game->GetGUIRenderMode();
		if (mode != GUIHOME)
		{
			GLFWwindow* aWindow = game->GetContext().getGLFWWindow();
			game->ToggleRenderMode(GUISETTINGS);
			bool s = game->GetRenderMode(GUISETTINGS);
			if (s)
			{
				int width;
				int height;
				glfwGetFramebufferSize(game->GetContext().getGLFWWindow(), &width, &height);
				glfwSetCursorPos(aWindow, width / 2, height / 2);
			}
			glfwSetInputMode(aWindow, GLFW_CURSOR, s ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
		}
	}

	void GUI::makeGUI()
	{
		int width;
		int height;
		glfwGetFramebufferSize(game->GetContext().getGLFWWindow(), &width, &height);
		Engine::RenderMode mode = game->GetGUIRenderMode();
		if (functions.find(mode) != functions.end())
		{
			functions[mode](&width, &height);
			ImGui::Render();
		}
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
			game->GetRenderer().initialiseJointMatrices();
			game->ToggleRenderMode(GUIHOME);
			game->ToggleRenderMode(FORWARD);
			//game->ToggleRenderMode(SHADOWS);
			GLFWwindow* window = game->GetContext().getGLFWWindow();
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		if (ImGui::Button("Multi-Player", ImVec2(*w / 4, *h / 4)))
		{
			game->ToggleRenderMode(GUIHOME);
			game->ToggleRenderMode(FORWARD);
			GLFWwindow* window = game->GetContext().getGLFWWindow();
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(window, *w / 2.0, *h / 2.0);
		}

		ImVec2 windowSize = ImVec2(*w / 4, *h / 4);
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 topRightPos = ImVec2(*w - *w / 6 - 10, 30); // Adjust for button size

		// Set cursor to top-right position
		ImGui::SetCursorPos(topRightPos);
		// ICON_FA_WRENCH
		if (ImGui::Button("Settings", ImVec2(*w / 6, *h / 6)))
		{
			game->ToggleRenderMode(GUIHOME);
			game->ToggleRenderMode(GUISETTINGS);
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

		ImGui::Text("Toggle Forward or Deferred rendering", ImVec2(*w / 4, *h / 4));
		bool forward = game->GetRenderMode(FORWARD);
		if (ImGui::Button(forward ? "Forward" : "Deferred", ImVec2(*w / 4, *h / 4)))
		{
			game->ToggleRenderMode(FORWARD);
			game->ToggleRenderMode(DEFERRED);
		}
		ImGui::Text("Toggle Shadows (Only available when in forward rendering mode)", ImVec2(*w / 4, *h / 4));
		//ImGui::Text("SHADOWS CURRENTLY ALWAYS ON IF FORWARD MODE", ImVec2(*w / 4, *h / 4));

		if (forward)
		{
			if (ImGui::Button(game->GetRenderMode(SHADOWS) ? "Shadows" : "No Shadows", ImVec2(*w / 4, *h / 4)))
			{
				game->ToggleRenderMode(SHADOWS);
			}
		}

		ImVec2 windowSize = ImVec2(*w / 4, *h / 4);
		ImVec2 windowPos = ImGui::GetWindowPos();
		ImVec2 topRightPos = ImVec2(*w - *w / 6 - 10, 30); // Adjust for button size

		// Set cursor to top-right position
		ImGui::SetCursorPos(topRightPos);
		if (ImGui::Button("Home", ImVec2(*w / 6, *h / 6)))
		{
			if (game->GetRenderer().GetIsSceneLoaded())
			{
				game->ResetRenderModes();
				game->GetEntityManager().ClearManager();
				//game->GetRenderer().cleanModelMatrices();
			}
			else
			{
				std::cout << game->GetRenderer().GetIsSceneLoaded() << std::endl;
			}
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

		ImGui::Text("Shadow depth buffer settings:");
		ImGui::SliderFloat("Depth Bias Constant", &game->GetRenderer().depthBiasConstant, 0.0f, 10.0f);
		ImGui::SliderFloat("Depth Bias Slope Factor", &game->GetRenderer().depthBiasSlopeFactor, 0.0f, 10.0f);

		ImGui::Text("Animations:");
		// Iterate over all models and find ones with animations
		std::vector<vk::Model>& models = game->GetModels();
		for (vk::Model& model : models) {
			if (model.animations.size() == 0)
				continue;

			// Get the list of animation names
			std::vector<const char*> list;
			std::size_t size = model.animations.size();
			list.reserve(size);
			for (std::size_t i = 0; i < size; i++)
				list.push_back(model.animations[i].name.c_str());

			ImGui::Combo("Animation", &model.animationIndex, list.data(), size, size);
			if (ImGui::Button("Play Animation")) {
				if (!model.animations[model.animationIndex].animating)
					model.animations[model.animationIndex].animating = true;
			}
		}

		ImGui::End();
	}
}
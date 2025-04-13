#include "GUI.hpp"
#include "../Network/Helpers/GameConfig.hpp"

struct GLFWwindow;

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
		if (mode != GUIHOME && mode != GUISERVER)
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
		ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));

		ImGui::Begin("Home Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		ImGui::Text("Demo game made using Heatstroke", ImVec2(*w / 4, *h / 4));
		if (ImGui::Button("Single Player", ImVec2(*w / 4, *h / 4)))
		{
			game->loadOnlineEntities();
			/*uint8_t b[1000];
			game->GetEntityManager().GetAllData(b);
			game->GetEntityManager().ClearManager();
			game->GetEntityManager().SetAllData(b);
			EntityManager* manager = &(game->GetEntityManager());
			std::vector<int> vec = manager->GetEntitiesWithComponent(PHYSICS);
			PhysicsComponent* comp;
			for (int i = 0; i < vec.size(); i++)
			{
				comp = reinterpret_cast<PhysicsComponent*>(manager->GetComponentOfEntity(vec[i], PHYSICS));
				glm::mat4 mat = manager->GetEntity(vec[i])->GetModelMatrix();
				PhysicsComponent::PhysicsType type = comp->GetPhysicsType();
				Engine::vk::Model& model = game->GetModels()[vec[i]];
				if (type == PhysicsComponent::PhysicsType::STATIC)
				{
					comp->InitComplexShape(game->GetPhysicsWorld(), type, model, mat, vec[i]);
				}
				else
				{
					comp->Init(game->GetPhysicsWorld(), type, model, mat, vec[i]);
				}
			}
			std::vector<int> entitiesWithNetworkComponent = manager->GetEntitiesWithComponent(NETWORK);
			NetworkComponent* networkComponent;
			for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
			{
				networkComponent = reinterpret_cast<NetworkComponent*>(manager->GetComponentOfEntity(entitiesWithNetworkComponent[i], NETWORK));
				if (networkComponent->GetClientId() == 0)
				{
					CameraComponent* cameraComponent = reinterpret_cast<CameraComponent*>(manager->GetComponentOfEntity(entitiesWithNetworkComponent[i], CAMERA));
					game->GetRenderer().attachCameraComponent(cameraComponent);
					break;
				}
			}*/
			game->GetRenderer().initialiseModelMatrices();
			game->GetRenderer().initialiseJointMatrices();
			game->ToggleRenderMode(GUIHOME);
			game->ToggleRenderMode(FORWARD);
			GLFWwindow* window = game->GetContext().getGLFWWindow();
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		if (ImGui::Button("Multi-Player", ImVec2(*w / 4, *h / 4)))
		{
			if (multiplayerSelected)
			{
				multiplayerSelected = false;
				serverSelected = false;
			}
			else
			{
				multiplayerSelected = true;
				serverSelected = false;
			}
		}
		if (ImGui::Button("Create a Server", ImVec2(*w / 4, *h / 4)))
		{
			if (serverSelected)
			{
				multiplayerSelected = false;
				serverSelected = false;
			}
			else
			{
				multiplayerSelected = false;
				serverSelected = true;
			}
		}

		ImVec2 middlePos = ImVec2(*w / 4, *h / 4);
		ImGui::SetCursorPos(middlePos);
		ImVec2 childSize = ImVec2(*w / 2, *h / 2);
		ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.3f, 0.4f, 1.0f));

		if (multiplayerSelected)
		{
			ImGui::BeginChild("MultiplayerBox", childSize, true, ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

			ImGui::Text("Join a server");

			static char addressStr[16] = "192.168.68.60\0";
			ImGui::Text("Address:");
			ImGui::InputText("Address", addressStr, IM_ARRAYSIZE(addressStr));

			static char portStr[6] = "";
			ImGui::Text("Port:");
			ImGui::InputText("Port", portStr, IM_ARRAYSIZE(portStr));
			int portNum = atoi(portStr);

			if (ImGui::Button("Go", ImVec2(40, 40)))
			{
				if (strlen(addressStr) == 0)
				{
					errorMsg = "Error: Address cannot be empty.";
				}
				else if (strlen(portStr) == 0)
				{
					errorMsg = "Error: Port cannot be empty.";
				}
				else if (!std::regex_match(addressStr, std::regex("^(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$")))
				{
					errorMsg = "Error: Invalid Address. " + std::string(addressStr) + " not of [www].[xxx].[yyy].[zzz] form";
				}
				else if (portNum < 1 || portNum > 65535)
				{
					errorMsg = "Error: Invalid Port number. " + std::string(portStr) + " not between 1 and 65535.";
				}
				else
				{
					errorMsg = "";
					game->ToggleRenderMode(GUIHOME);
					game->ToggleRenderMode(GUILOADING);
					yojimbo::Address address = yojimbo::Address(addressStr, portNum);
					game->SetClient(address);
				}
			}
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), errorMsg.c_str());

			ImGui::EndChild();
		}
		else if (serverSelected)
		{
			ImGui::BeginChild("ServerBox", childSize, true, ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

			ImGui::Text("Start a server");

			static char portStr[6] = "";
			ImGui::Text("Port:");
			ImGui::InputText("Port", portStr, IM_ARRAYSIZE(portStr));
			int portNum = atoi(portStr);

			static char maxClientsStr[6] = "";
			ImGui::Text("Max Clients:");
			ImGui::InputText("Max Clients", maxClientsStr, IM_ARRAYSIZE(maxClientsStr));
			int maxClientsNum = atoi(maxClientsStr);

			if (ImGui::Button("Go", ImVec2(40, 40)))
			{
				if (strlen(portStr) == 0)
				{
					errorMsg = "Error: Port cannot be empty.";
				}
				else if (strlen(maxClientsStr) == 0)
				{
					errorMsg = "Error: Max Clients cannot be empty.";
				}
				else if (portNum < 1 || portNum > 65535)
				{
					errorMsg = "Error: Invalid Port number. " + std::string(portStr) + " not between 1 and 65535.";
				}
				else if (maxClientsNum < 1 || maxClientsNum > 50)
				{
					errorMsg = "Error: Invalid Max Clients number. " + std::string(maxClientsStr) + " not between 1 and 50.";
				}
				else
				{
					errorMsg = "";
					game->loadOnlineEntities();
					game->GetRenderer().initialiseModelMatrices();
					game->GetRenderer().initialiseJointMatrices();
					game->ToggleRenderMode(GUIHOME);
					game->ToggleRenderMode(GUISERVER);
					game->SetServer(portNum, maxClientsNum);
				}
			}
			ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), errorMsg.c_str());

			ImGui::EndChild();
		}

		ImGui::PopStyleColor();

		ImGui::End();

		ImGui::PopStyleColor(4);
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

		ImVec2 topRightPos = ImVec2(*w - *w / 6 - 10, 30);
		ImGui::SetCursorPos(topRightPos);
		if (ImGui::Button("Disconnect", ImVec2(*w / 6, *h / 6)))
		{
			if (game->GetRenderer().GetIsSceneLoaded())
			{
				game->ResetRenderModes();
				game->GetEntityManager().ClearManager();
				game->GetRenderer().cleanModelMatrices();
				game->GetNetwork().Reset();
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

		glm::vec3 pos = game->GetRenderer().GetCameraPointer()->position;
		glm::vec3 fDir = game->GetRenderer().GetCameraPointer()->frontDirection;
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
				model.animationQueue.push(model.animations[model.animationIndex]);
				model.blending = true;
			}
		}

		if (game->GetNetwork().GetStatus() != Status::NETWORK_UNINITIALIZED)
		{
			std::map<std::string, std::string> networkInfo = game->GetNetwork().GetNetworkInfo();
			for (const auto& [key, value] : networkInfo)
			{
				ImGui::Text("%s: %s", key.c_str(), value.c_str());
			}
		}

		ImGui::End();
	}

	void GUI::makeServerGUI(int* w, int* h)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec4 clear_color = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(*w, *h));

		ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
		ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));

		ImGui::Begin("Server Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		std::map<std::string, std::string> networkInfo = game->GetNetwork().GetNetworkInfo();
		for (const auto& [key, value] : networkInfo)
		{
			ImGui::Text("%s: %s", key.c_str(), value.c_str());
		}

		if (ImGui::Button("Stop server", ImVec2(*w / 6, *h / 6)))
		{
			game->ResetRenderModes();
			game->GetEntityManager().ClearManager();
			game->GetRenderer().cleanModelMatrices();
			game->GetNetwork().Reset();
			multiplayerSelected = false;
			serverSelected = false;
		}

		ImGui::End();

		ImGui::PopStyleColor(3);
	}

	void GUI::makeLoadingGUI(int* w, int* h)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImVec4 clear_color = ImVec4(0.2f, 0.2f, 0.2f, 1.00f);

		ImGui_ImplVulkan_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		ImGui::SetNextWindowPos(ImVec2(0, 0));
		ImGui::SetNextWindowSize(ImVec2(*w, *h));

		ImGui::Begin("Loading Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

		Status s = game->GetNetwork().GetStatus();
		if (s == Status::CLIENT_LOADED)
		{
			game->GetRenderer().initialiseModelMatrices();
			game->GetRenderer().initialiseJointMatrices();
			game->ToggleRenderMode(GUILOADING);
			game->ToggleRenderMode(FORWARD);
			GLFWwindow* window = game->GetContext().getGLFWWindow();
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}
		else if (s == Status::CLIENT_DISCONNECTED || s == Status::CLIENT_CONNECTION_FAILED)
		{
			ImGui::Text("Loading Failed.");
			ImGui::Text(game->GetNetwork().GetStatusString().c_str());
			ImVec2 topRightPos = ImVec2(*w - *w / 6 - 10, 30);
			ImGui::SetCursorPos(topRightPos);
			if (ImGui::Button("Home", ImVec2(*w / 6, *h / 6)))
			{
				game->ResetRenderModes();
				game->GetNetwork().Reset();
			}
		}
		else
		{
			ImGui::Text("Loading: ");
			ImGui::Text(game->GetNetwork().GetStatusString().c_str());
		}

		ImGui::End();
	}
}
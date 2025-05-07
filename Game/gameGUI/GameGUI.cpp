#include "gameGUI.hpp"
#include "../gameModes/GameMode.hpp"
#include "../gameModes/SinglePlayer.hpp"
#include "../gameModes/MultiPlayer.hpp"

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

using namespace Engine;

// Define the global variables here
bool debugInput = false;
bool debugGame = true;
bool multiplayerSelected = false;
bool serverSelected = false;
std::string errorMsg = "";
ImVec2 serverBoxSize = ImVec2(0, 0);
std::string loadingMsg = "Messages not yet setup. Need to put this onto a thread.";
Engine::Camera cameraTemp;
bool singleHovered = false;
bool multiHovered = false;
bool serverHovered = false;

void makeGameGUIS(FPSTest* game)
{
	GUI& gui = game->getGUI();

	gui.AddFunction("Home", [game](int* w, int* h) { makeHomeGUI(game, w, h); });
	gui.AddFunction("Settings", [game](int* w, int* h) { makeSettingsGUI(game, w, h); });
	gui.AddFunction("Server", [game](int* w, int* h) { makeServerGUI(game, w, h); });
	gui.AddFunction("Loading", [game](int* w, int* h) { makeLoadingGUI(game, w, h); });
	gui.AddFunction("Debug", [game](int* w, int* h) { makeDebugGUI(game, w, h); });
	gui.AddFunction("SinglePlayer", [game](int* w, int* h) { makeSinglePlayerGUI(game, w, h); });
	gui.AddFunction("MultiPlayer", [game](int* w, int* h) { makeMultiPlayerGUI(game, w, h); });

	gui.AddFont("Default", "Engine/third_party/imgui/misc/fonts/Roboto-Medium.ttf", 12.0f);
	gui.AddFont("Home", "Engine/third_party/imgui/misc/fonts/Freedom.ttf", 60.0f);
	gui.AddFont("HomeHovered", "Engine/third_party/imgui/misc/fonts/Freedom.ttf", 70.0f);
	gui.AddFont("Game", "Engine/third_party/imgui/misc/fonts/Roboto-Medium.ttf", 36.0f);

	gui.AddTexture("Test", "Game/assets/Assets/maps/russian_house/textures/Balkon_01_baseColor.png");

	gui.ToggleGUIMode("Home");
}

void makeHomeGUI(FPSTest* game, int* w, int* h)
{
	ImGuiViewport* viewport = ImGui::GetMainViewport();

	ImGui::SetNextWindowPos(viewport->Pos, ImGuiCond_Always);
	ImGui::SetNextWindowSize(viewport->Size, ImGuiCond_Always);

	// Flags to remove decorations and background
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoInputs |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoSavedSettings |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoBackground;

	ImGui::Begin("BackgroundImage", nullptr, window_flags);
	ImGui::SetCursorPos(ImVec2(0.f, 0.f));
	ImGui::Image((ImTextureID)std::get<3>(*game->getGUI().GetImage("Test")), viewport->Size);
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(*w, *h));

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));

	ImGui::Begin("Home Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	ImGui::Text("Demo game made using Heatstroke", ImVec2(*w / 4, *h / 4));

	ImGui::PushFont(game->getGUI().GetFont("Home"));

	const char* label = "Single Player";
	ImVec2 textSize;
	ImVec2 boxSize;
	ImVec2 cursorPos;
	ImU32 color = ImGui::GetColorU32(ImGuiCol_Text);

	if (singleHovered)
	{
		ImGui::PushFont(game->getGUI().GetFont("HomeHovered"));
		textSize = ImGui::CalcTextSize(label);
		boxSize = ImVec2(textSize.x - 10.f, textSize.y - 30.f);

		ImGui::SetCursorPosX(20.f);
		ImVec2 temp = ImGui::GetCursorScreenPos();
		temp.y += 20.f;
		ImGui::SetCursorPos(temp);

		if (ImGui::InvisibleButton("##singlebtn", boxSize))
		{
			game->SetGameMode(std::make_unique<SinglePlayer>(game));
			game->loadOfflineEntities();
			game->getRenderer().initialiseJointMatrices();
			game->getGUI().ToggleGUIMode("Home");
			game->getGUI().ToggleGUIMode("SinglePlayer");
			if (game->debugging)
				game->getGUI().ToggleGUIMode("Debug");
			game->SetRenderMode(RenderMode::FORWARD);
			GLFWwindow* window = game->GetContext().getGLFWWindow();
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.x += 20.f;
		cursorPos.y -= boxSize.y + 4;
		cursorPos.x += (boxSize.x - textSize.x) * 0.5f;
		cursorPos.y += (boxSize.y - textSize.y) * 0.5f;
		color = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::GetWindowDrawList()->AddText(cursorPos, color, label);

		ImGui::PopFont();

		if (!ImGui::IsItemHovered())
			singleHovered = false;
	}
	else
	{
		textSize = ImGui::CalcTextSize(label);
		boxSize = ImVec2(textSize.x - 10.f, textSize.y - 30.f);

		ImGui::SetCursorPosX(20.f);
		ImVec2 temp = ImGui::GetCursorScreenPos();
		temp.y += 20.f;
		ImGui::SetCursorPos(temp);

		if (ImGui::InvisibleButton("##singlebtn", boxSize))
		{
			game->SetGameMode(std::make_unique<SinglePlayer>(game));
			game->loadOfflineEntities();
			game->getRenderer().initialiseJointMatrices();
			game->getGUI().ToggleGUIMode("Home");
			game->getGUI().ToggleGUIMode("SinglePlayer");
			if (game->debugging)
				game->getGUI().ToggleGUIMode("Debug");
			game->SetRenderMode(RenderMode::FORWARD);
			GLFWwindow* window = game->GetContext().getGLFWWindow();
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		}

		cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.x += 20.f;
		cursorPos.y -= boxSize.y + 4;
		cursorPos.x += (boxSize.x - textSize.x) * 0.5f;
		cursorPos.y += (boxSize.y - textSize.y) * 0.5f;
		color = ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
		ImGui::GetWindowDrawList()->AddText(cursorPos, color, label);

		if (ImGui::IsItemHovered())
			singleHovered = true;
	}

	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	label = "Multi-Player";

	if (multiHovered)
	{
		ImGui::PushFont(game->getGUI().GetFont("HomeHovered"));
		textSize = ImGui::CalcTextSize(label);
		boxSize = ImVec2(textSize.x - 10.f, textSize.y - 30.f);

		ImGui::SetCursorPosX(20.f);
		ImVec2 temp = ImGui::GetCursorScreenPos();
		temp.y += 20.f;
		ImGui::SetCursorPos(temp);

		if (ImGui::InvisibleButton("##multibtn", boxSize))
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

		cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.x += 20.f;
		cursorPos.y -= boxSize.y + 4;
		cursorPos.x += (boxSize.x - textSize.x) * 0.5f;
		cursorPos.y += (boxSize.y - textSize.y) * 0.5f;
		color = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::GetWindowDrawList()->AddText(cursorPos, color, label);

		ImGui::PopFont();

		if (!ImGui::IsItemHovered())
			multiHovered = false;
	}
	else
	{
		textSize = ImGui::CalcTextSize(label);
		boxSize = ImVec2(textSize.x - 10.f, textSize.y - 30.f);

		ImGui::SetCursorPosX(20.f);
		ImVec2 temp = ImGui::GetCursorScreenPos();
		temp.y += 20.f;
		ImGui::SetCursorPos(temp);

		if (ImGui::InvisibleButton("##multibtn", boxSize))
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

		cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.x += 20.f;
		cursorPos.y -= boxSize.y + 4;
		cursorPos.x += (boxSize.x - textSize.x) * 0.5f;
		cursorPos.y += (boxSize.y - textSize.y) * 0.5f;
		color = ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
		ImGui::GetWindowDrawList()->AddText(cursorPos, color, label);

		if (ImGui::IsItemHovered())
			multiHovered = true;
	}

	ImGui::Dummy(ImVec2(0.0f, 20.0f));

	label = "Create a server";

	if (serverHovered)
	{
		ImGui::PushFont(game->getGUI().GetFont("HomeHovered"));
		textSize = ImGui::CalcTextSize(label);
		boxSize = ImVec2(textSize.x - 10.f, textSize.y - 30.f);

		ImGui::SetCursorPosX(20.f);
		ImVec2 temp = ImGui::GetCursorScreenPos();
		temp.y += 20.f;
		ImGui::SetCursorPos(temp);

		if (ImGui::InvisibleButton("##serverbtn", boxSize))
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

		cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.x += 20.f;
		cursorPos.y -= boxSize.y + 4;
		cursorPos.x += (boxSize.x - textSize.x) * 0.5f;
		cursorPos.y += (boxSize.y - textSize.y) * 0.5f;
		color = ImGui::GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
		ImGui::GetWindowDrawList()->AddText(cursorPos, color, label);

		ImGui::PopFont();

		if (!ImGui::IsItemHovered())
			serverHovered = false;
	}
	else
	{
		textSize = ImGui::CalcTextSize(label);
		boxSize = ImVec2(textSize.x - 10.f, textSize.y - 30.f);

		ImGui::SetCursorPosX(20.f);
		ImVec2 temp = ImGui::GetCursorScreenPos();
		temp.y += 20.f;
		ImGui::SetCursorPos(temp);

		if (ImGui::InvisibleButton("##serverbtn", boxSize))
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

		cursorPos = ImGui::GetCursorScreenPos();
		cursorPos.x += 20.f;
		cursorPos.y -= boxSize.y + 4;
		cursorPos.x += (boxSize.x - textSize.x) * 0.5f;
		cursorPos.y += (boxSize.y - textSize.y) * 0.5f;
		color = ImGui::GetColorU32(ImVec4(0.7f, 0.7f, 0.7f, 1.0f));
		ImGui::GetWindowDrawList()->AddText(cursorPos, color, label);

		if (ImGui::IsItemHovered())
			serverHovered = true;
	}

	ImVec2 middlePos = ImVec2(*w / 4, *h / 4);
	ImGui::SetCursorPos(middlePos);
	ImVec2 childSize = ImVec2(*w / 2, *h / 2);
	ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.2f, 0.3f, 0.4f, 1.0f));

	if (multiplayerSelected)
	{
		ImGui::BeginChild("MultiplayerBox", childSize, true, ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		ImGui::PushFont(game->getGUI().GetFont("Default"));

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
				game->getGUI().ToggleGUIMode("Home");
				game->getGUI().ToggleGUIMode("Loading");
				yojimbo::Address address = yojimbo::Address(addressStr, portNum);
				game->SetClient(address);
			}
		}
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), errorMsg.c_str());

		ImGui::PopFont();

		ImGui::EndChild();
	}
	else if (serverSelected)
	{
		ImGui::BeginChild("ServerBox", childSize, true, ImGuiWindowFlags_ChildWindow | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

		ImGui::PushFont(game->getGUI().GetFont("Default"));

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
				game->loadOnlineEntities(maxClientsNum);
				game->getRenderer().initialiseJointMatrices();
				game->getGUI().ToggleGUIMode("Home");
				game->getGUI().ToggleGUIMode("Server");
				game->SetServer(portNum, maxClientsNum);
			}
		}
		ImGui::TextColored(ImVec4(1.0f, 0.0f, 0.0f, 1.0f), errorMsg.c_str());

		ImGui::PopFont();

		ImGui::EndChild();
	}

	ImGui::PopStyleColor(5);
	ImGui::PopFont();

	ImGui::End();
}

void makeSettingsGUI(FPSTest* game, int* w, int* h)
{
	ImGui::PushFont(game->getGUI().GetFont("Default"));

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(*w, *h));

	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.5f, 0.1f, 0.1f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.8f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 0.1f, 0.1f, 1.0f));

	ImGui::Begin("Settings Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	// Deferred rendering is deprecated for now
	//const char* renderModes[] = { "Forward", "Deferred", "Forward + Shadows" };
	const char* renderModes[] = { "Forward" };
	RenderMode current = game->getRenderMode();

	int selected = 0;
	switch (current) {
	case RenderMode::FORWARD: selected = 0; break;
	//case RenderMode::DEFERRED: selected = 1; break;
	}

	// Render mode combo box
	ImGui::Text("Render Mode:");
	if (ImGui::Combo("##RenderMode", &selected, renderModes, IM_ARRAYSIZE(renderModes))) {
		switch (selected) {
		case 0: game->SetRenderMode(RenderMode::FORWARD); break;
		//case 1: game->SetRenderMode(RenderMode::DEFERRED); break;
		}
	}

	if (ImGui::Checkbox("Shadows Enabled", &game->getRenderer().getShadowState())) {
		// When shadows are toggled, we need to recreate the swapchain
		game->getRenderer().setRecreateSwapchain(true);
	}

	//// Shadow toggle display (reflects render mode but can't be toggled directly)
	//bool shadowsEnabled = (selected == 2);
	//ImGui::BeginDisabled(); // Shadows are implied by mode, not toggleable separately
	//ImGui::Checkbox("Shadows Enabled", &shadowsEnabled);
	//ImGui::EndDisabled();

	Renderer& renderer = game->getRenderer();

	ImGui::Text("Anti Aliasing:");
	std::pair<const char**, int> msaaOptions = renderer.getMSAAOptions();
	if (ImGui::Combo("MSAA:", &renderer.getMSAAIndex(), msaaOptions.first, msaaOptions.second, msaaOptions.second)) {
		renderer.setRecreateSwapchain(true);
	}

	ImGui::Text("Crosshair Color:");
	Crosshair& crosshair = game->GetCrosshair();
	if (ImGui::Combo("Color", &crosshair.selectedColor, crosshair.colorNames.data(), (int)crosshair.colorNames.size())) {
		crosshair.shouldUpdateCrosshair = true;
	}

	ImGui::Text("Sensitivity:");

	float sensitivity = renderer.getCameraPointer()->sensitivity;
	int sensitivityInt = static_cast<int>(sensitivity * 100);

	// Slider from 0.01 to 1.00
	ImGui::PushItemWidth(*w / 4.f);
	if (ImGui::SliderInt("##SensitivitySlider", &sensitivityInt, 1, 100))
	{
		sensitivity = sensitivityInt / 100.f;
		sensitivity = std::clamp(sensitivity, 0.01f, 1.0f);
		renderer.getCameraPointer()->sensitivity = sensitivity;
	}
	ImGui::PopItemWidth();

	ImGui::SameLine();

	ImVec2 size = ImGui::CalcTextSize("0.00");
	ImGui::PushItemWidth(size.x + ImGui::GetStyle().FramePadding.x * 2);
	if (ImGui::InputInt("##SensitivityInput", &sensitivityInt, 0, 0))
	{
		sensitivity = sensitivityInt / 100.f;
		sensitivity = std::clamp(sensitivity, 0.01f, 1.0f);
		renderer.getCameraPointer()->sensitivity = sensitivity;
	}
	ImGui::PopItemWidth();

	ImVec2 topRightPos = ImVec2(*w - *w / 6 - 10, 30);
	ImGui::SetCursorPos(topRightPos);
	if (ImGui::Button("Disconnect", ImVec2(*w / 6, *h / 6)))
	{
		if (renderer.getIsSceneLoaded())
		{
			game->getGUI().ResetGUIModes();
			game->getGUI().ToggleGUIMode("Home");
			game->GetPhysicsWorld().reset(&game->GetEntityManager());
			game->SetRenderMode(RenderMode::NO_DATA_MODE);
			game->SetGameMode(nullptr);
			game->GetEntityManager().ClearManager();
			game->GetNetwork().Reset();
		}
	}

	ImGui::End();

	ImGui::PopStyleColor(3);
	ImGui::PopFont();
}

void makeServerGUI(FPSTest* game, int* w, int* h)
{
	ImGui::PushFont(game->getGUI().GetFont("Default"));

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
		game->SetRenderMode(RenderMode::NO_DATA_MODE);
		game->GetEntityManager().ClearManager();
		game->GetNetwork().Reset();
		multiplayerSelected = false;
		serverSelected = false;
	}

	ImGui::End();

	ImGui::PopStyleColor(3);
	ImGui::PopFont();
}

void makeLoadingGUI(FPSTest* game, int* w, int* h)
{
	ImGui::PushFont(game->getGUI().GetFont("Default"));

	ImGui::SetNextWindowPos(ImVec2(0, 0));
	ImGui::SetNextWindowSize(ImVec2(*w, *h));

	ImGui::Begin("Loading Menu", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

	Status s = game->GetNetwork().GetStatus();
	if (s == Status::CLIENT_INITIALIZING_DATA)
	{
		game->getRenderer().initialiseJointMatrices();
		game->getGUI().ToggleGUIMode("Loading");
		if (game->debugging)
			game->getGUI().ToggleGUIMode("Debug");
		game->SetRenderMode(RenderMode::FORWARD);
		GLFWwindow* window = game->GetContext().getGLFWWindow();
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
	else if (s == Status::CLIENT_CONNECTION_FAILED)
	{
		ImGui::Text("Loading Failed.");
		ImGui::Text(game->GetNetwork().GetStatusString().c_str());
		ImVec2 topRightPos = ImVec2(*w - *w / 6 - 10, 30);
		ImGui::SetCursorPos(topRightPos);
		if (ImGui::Button("Home", ImVec2(*w / 6, *h / 6)))
		{
			game->getGUI().ToggleGUIMode("Home");
			if (game->debugging)
				game->getGUI().ToggleGUIMode("Debug");
			game->GetNetwork().Reset();
		}
	}
	else if (s == Status::CLIENT_DISCONNECTED)
	{
		ImGui::Text("Client disconnected from server.");
		ImGui::Text(game->GetNetwork().GetStatusString().c_str());
		ImVec2 topRightPos = ImVec2(*w - *w / 6 - 10, 30);
		ImGui::SetCursorPos(topRightPos);
		if (ImGui::Button("Home", ImVec2(*w / 6, *h / 6)))
		{
			game->SetRenderMode(RenderMode::NO_DATA_MODE);
			game->getGUI().ToggleGUIMode("Home");
			if (game->debugging)
				game->getGUI().ToggleGUIMode("Debug");
			game->GetEntityManager().ClearManager();
			game->GetNetwork().Reset();
		}
	}
	else
	{
		ImGui::Text("Loading: ");
		ImGui::Text(game->GetNetwork().GetStatusString().c_str());
	}

	ImGui::End();

	ImGui::PopFont();
}

void makeDebugGUI(FPSTest* game, int* w, int* h)
{
	ImGui::PushFont(game->getGUI().GetFont("Default"));

	ImGui::Begin("Debug Menu");

	if (ImGui::Checkbox("VSync", &game->getRenderer().vsync)) {
		game->getRenderer().setRecreateSwapchain(true);
	}

	ImGui::Text("List of info would go here", ImVec2(*w / 4, *h / 4));

	glm::vec3 pos = game->getRenderer().getCameraPointer()->position;
	glm::vec3 fDir = game->getRenderer().getCameraPointer()->frontDirection;
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

	if (InputManager::hasJoysticksConnected())
	{
		ImGui::Text("Input:");
		ImGui::Text("Controller Status: %s", InputManager::hasJoysticksConnected() ? "Connected" : "Disconnected");
		if (InputManager::hasJoysticksConnected())
		{
			ImGui::Text("A: %s", InputManager::getJoystick(0).isPressed(HS_GAMEPAD_BUTTON_A) ? "Pressed" : "Released");
			ImGui::Text("B: %s", InputManager::getJoystick(0).isPressed(HS_GAMEPAD_BUTTON_B) ? "Pressed" : "Released");
			ImGui::Text("Y: %s", InputManager::getJoystick(0).isPressed(HS_GAMEPAD_BUTTON_Y) ? "Pressed" : "Released");
			ImGui::Text("X: %s", InputManager::getJoystick(0).isPressed(HS_GAMEPAD_BUTTON_X) ? "Pressed" : "Released");
			ImGui::Text("RB: %s", InputManager::getJoystick(0).isPressed(HS_GAMEPAD_BUTTON_RIGHT_BUMPER) ? "Pressed" : "Released");
			ImGui::Text("LB: %s", InputManager::getJoystick(0).isPressed(HS_GAMEPAD_BUTTON_LEFT_BUMPER) ? "Pressed" : "Released");
			ImGui::Text("RT: %f", InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_TRIGGER));
			ImGui::Text("LT: %f", InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_LEFT_TRIGGER));
			ImGui::Text("LS - Horizontal: %f", InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_LEFT_X));
			ImGui::Text("LS - Vertical: %f", InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_LEFT_Y));
			ImGui::Text("RS - Horizontal: %f", InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_X));
			ImGui::Text("RS - Vertical: %f", InputManager::getJoystick(0).getAxisValue(HS_GAMEPAD_AXIS_RIGHT_Y));
		}
	}

	ImGui::Text("Shadow depth buffer settings:");
	ImGui::SliderFloat("Depth Bias Constant", &game->getRenderer().depthBiasConstant, 0.0f, 10.0f);
	ImGui::SliderFloat("Depth Bias Slope Factor", &game->getRenderer().depthBiasSlopeFactor, 0.0f, 10.0f);

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

		ImGui::Combo("Animation", &model.animationIndex, list.data(), (int)size, (int)size);
		if (ImGui::Button("Play Animation")) {
			model.playAnimation();
		}
	}

	ImGui::Text("Active Decals: %d/100", game->getDecals().getNbActiveDecals());

	if (game->GetNetwork().GetStatus() != Status::NETWORK_UNINITIALIZED)
	{
		std::map<std::string, std::string> networkInfo = game->GetNetwork().GetNetworkInfo();
		for (const auto& [key, value] : networkInfo)
		{
			ImGui::Text("%s: %s", key.c_str(), value.c_str());
		}
	}

	ImGui::End();

	ImGui::PopFont();
}

void makeSinglePlayerGUI(FPSTest* game, int*, int*)
{
	SinglePlayer* sp = dynamic_cast<SinglePlayer*>(&game->GetGameMode());
	if (sp)
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
		bool test = true;

		ImVec2 windowPadding = ImVec2(10, 10);

		// Get current window size
		ImVec2 displaySize = ImGui::GetIO().DisplaySize;

		// Set window position to top-right (with padding)
		ImGui::SetNextWindowPos(
			ImVec2(displaySize.x - windowPadding.x, windowPadding.y),
			ImGuiCond_Always,
			ImVec2(1.0f, 0.0f) // Pivot (1, 0) means align from top-right corner
		);

		ImGui::PushFont(game->getGUI().GetFont("Game"));
		ImGui::Begin("Game:", &test, window_flags);

		ImGui::Text("SCORE: %u", sp->score);
		ImGui::Text("Time: %u", sp->countdown);

		ImGui::End();
		ImGui::PopFont();
	}
}

void makeMultiPlayerGUI(FPSTest* game, int*, int*)
{
	MultiPlayer* mp = dynamic_cast<MultiPlayer*>(&game->GetGameMode());
	if (mp)
	{
		ImGuiWindowFlags window_flags = 0;
		window_flags |= ImGuiWindowFlags_NoBackground;
		window_flags |= ImGuiWindowFlags_NoTitleBar;
		bool test = true;

		ImGui::PushFont(game->getGUI().GetFont("Game"));
		ImGui::Begin("Multi Player:", &test, window_flags);

		ImGui::End();
		ImGui::PopFont();
	}
}

void toggleSettings(FPSTest* game)
{
	RenderMode mode = game->getRenderMode();
	GUI& gui = game->getGUI();
	if (mode == NO_DATA_MODE)
	{
		multiplayerSelected = false;
		serverSelected = false;
	}
	else
	{
		GLFWwindow* aWindow = game->GetContext().getGLFWWindow();
		gui.ToggleGUIMode("Settings");
		bool s = gui.GetGUIMode("Settings");
		if (s)
		{
			CameraComponent* comp = reinterpret_cast<CameraComponent*>(game->GetEntityManager().GetComponentOfEntity(game->GetGameMode().GetPlayerEntity()->GetEntityId(), CAMERA));
			cameraTemp = *comp->GetCamera();
			glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		}
		else
		{
			float x = cameraTemp.lastX;
			float y = cameraTemp.lastY;
			glfwSetInputMode(aWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(aWindow, x, y);
		}

	}
}
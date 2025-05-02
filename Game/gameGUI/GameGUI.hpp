#pragma once

#include "../DemoGame.hpp"

#include <vector>
#include <string>

namespace Engine
{
	class Game;
	class InputManager;
}

void makeGameGUIS(FPSTest* game);
void makeHomeGUI(FPSTest* game, int*, int*);
void makeSettingsGUI(FPSTest* game, int*, int*);
void makeServerGUI(FPSTest* game, int*, int*);
void makeLoadingGUI(FPSTest* game, int*, int*);
void makeDebugGUI(FPSTest* game, int*, int*);
void makeSinglePlayerGUI(FPSTest* game, int*, int*);
void makeMultiPlayerGUI(FPSTest* game, int*, int*);

void toggleSettings(FPSTest* game);

// from input actions
bool debugInput{ false };
bool debugGame{ true };
// from input actions

// Pop-up info
bool multiplayerSelected = false;
bool serverSelected = false;
std::string errorMsg = "";
ImVec2 serverBoxSize = ImVec2(0, 0);

// Single player

// Loading info
std::string loadingMsg = "Messages not yet setup. Need to put this onto a thread.";

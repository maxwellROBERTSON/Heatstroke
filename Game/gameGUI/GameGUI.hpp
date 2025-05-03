#pragma once

#include "../DemoGame.hpp"

#include <string>

void makeGameGUIS(FPSTest* game);
void makeHomeGUI(FPSTest* game, int*, int*);
void makeSettingsGUI(FPSTest* game, int*, int*);
void makeServerGUI(FPSTest* game, int*, int*);
void makeLoadingGUI(FPSTest* game, int*, int*);
void makeDebugGUI(FPSTest* game, int*, int*);
void makeSinglePlayerGUI(FPSTest* game, int*, int*);
void makeMultiPlayerGUI(FPSTest* game, int*, int*);

void toggleSettings(FPSTest* game);

// Declare global variables as externs
extern bool debugInput;
extern bool debugGame;
extern bool multiplayerSelected;
extern bool serverSelected;
extern std::string errorMsg;
extern ImVec2 serverBoxSize;
extern std::string loadingMsg;
extern Engine::Camera cameraTemp;

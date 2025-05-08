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

void ShowInputDebug(FPSTest* game, int*, int*);
void ShowRendererDebug(FPSTest* game, int*, int*);
void ShowAnimationDebug(FPSTest* game, int*, int*);
void ShowNetworkDebug(FPSTest* game, int*, int*);
void ShowGameDebug(FPSTest* game, int*, int*);

void toggleSettings(FPSTest* game);

// Declare global variables as externs
extern bool showImGuiDemoWindow;
extern bool debugRenderer;
extern bool debugAnimations;
extern bool showGameGUI; //to add to debugging somehow
extern bool debugNetwork;
extern bool debugInput;
extern bool debugGame;
extern bool multiplayerSelected;
extern bool serverSelected;
extern std::string errorMsg;
extern ImVec2 serverBoxSize;
extern std::string loadingMsg;
extern Engine::Camera cameraTemp;
extern bool singleHovered;
extern bool multiHovered;
extern bool serverHovered;

#pragma once

#include "../Helpers/GameAdapter.hpp"

class GameClient
{
public:
	GameClient(
		yojimbo::ClientServerConfig*,
		GameAdapter*,
		yojimbo::Address
	);
	~GameClient() {}

	void Connect(yojimbo::Address);
	void Run();
	int Update();
	void ProcessMessages();
	void ProcessMessage(yojimbo::Message*);
	void CleanUp();

private:
	double time = 1.0;
	float dt = 1.0f / 120.0f;
	yojimbo::Client* client;

	yojimbo::ClientServerConfig* config;
	GameAdapter* adapter;
};

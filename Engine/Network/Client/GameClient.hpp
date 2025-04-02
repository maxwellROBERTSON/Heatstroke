#pragma once

#include "../Helpers/GameConfig.hpp"
#include "../Helpers/GameAdapter.hpp"
#include "../Network.hpp"

namespace Engine
{
	class Game;
}

namespace Engine
{
	class GameClient : public GameNetworkType
	{
	public:
		GameClient(
			yojimbo::ClientServerConfig*,
			GameAdapter*,
			yojimbo::Address,
			Engine::Game* game
		);
		~GameClient() {}

		void Connect();
		void Update();
		void ProcessMessages();
		void ProcessMessage(yojimbo::Message*);
		void HandleResponseMessage(RequestResponseMessage*);
		void CleanUp();

		void UpdateStatus();
		std::map<std::string, std::string> GetInfo();

	private:
		double time = 1.0;
		float dt = 1.0f / 120.0f;

		yojimbo::Address serverAddress;

		yojimbo::Client* client;
		yojimbo::ClientServerConfig* config;
		GameAdapter* adapter;

		Engine::Game* game;
	};
}

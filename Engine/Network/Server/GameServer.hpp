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
	class GameServer : public GameNetworkType
	{
	public:
		GameServer(
			yojimbo::ClientServerConfig* config,
			GameAdapter* adapter,
			yojimbo::Address address,
			int maxClients,
			Engine::Game* game
		);
		~GameServer() {}

		void Start();
		void Update();
		void ProcessMessages();
		void HandleRequestMessage(int, RequestType);
		void ProcessMessage(int, GameMessage*);
		void CleanUp();

		void UpdateStatus();
		std::map<std::string, std::string> GetInfo();

	private:
		float dt = 1.0f / 120.0f;
		int maxClients;

		yojimbo::Server* server;
		yojimbo::ClientServerConfig* config;
		GameAdapter* adapter;

		Engine::Game* mGame;
	};
}

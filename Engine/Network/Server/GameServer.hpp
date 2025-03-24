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
		void Run();
		void Update(float);
		void ProcessMessages();
		void ProcessMessage(int, GameMessage*);
		void CleanUp();

	private:
		int maxClients;
		yojimbo::Server* server;

		yojimbo::ClientServerConfig* config;
		GameAdapter* adapter;

		Engine::Game* mGame;
	};
}

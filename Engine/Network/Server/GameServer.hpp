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
		// Constructor
		GameServer(
			yojimbo::ClientServerConfig* config,
			GameAdapter* adapter,
			yojimbo::Address address,
			int maxClients,
			Engine::Game* game
		);
		~GameServer() {}

		// Start the server
		void Start();

		// Update server at a fixed rate, send and recieve packets, process messages from clients, advance server time
		void Update();

		// Loop through all client messages, process and release
		void ProcessMessages();

		// Process message type with corresponding function
		void ProcessMessage(int, yojimbo::Message*);

		// Handle request messages, given RequestType send correct data
		void HandleRequestMessage(int, RequestType);

		// Clean up server memory using yojimbo
		void CleanUp();

		// Update network status based on clients and server status
		void UpdateStatus();

		// Get debugging info for the server
		std::map<std::string, std::string> GetInfo();

	private:
		double serverTime;
		int maxClients;

		yojimbo::Server* server;
		yojimbo::ClientServerConfig* config;
		GameAdapter* adapter;

		Engine::Game* game;
	};
}

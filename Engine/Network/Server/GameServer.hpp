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

		// Handle a client update on entity data
		void HandleClientUpdateEntityData(int, ClientUpdateEntityData*);

		// Queue entity state messages to connected clients, if new client, send all data
		void QueueStateMessages();

		// Handle a request for entity data
		void HandleRequestEntityData(int);

		// Reset an entity and its component used by a disconnected client
		void ResetNetworkEntity(uint64_t);

		// Clean up server memory using yojimbo
		void CleanUp();

		// Update network status based on clients and server status
		void UpdateStatus();

		// Get debugging info for the server
		std::map<std::string, std::string> GetInfo();

	private:
		double serverTime;
		int maxClients;

		// Client sends a request for entity data, which moves it into the connection queue
		// Once server, sends the response for entity data, it is removed from the connection queue
		// It is only added to the loadedClients once a ClientInitialized message is recevied,
		// And then it is ready for frequent server update messages
		std::vector<std::pair<int, uint64_t>> connectionQueue;
		std::vector<std::pair<int, uint64_t>> loadedClients;

		yojimbo::Server* server;
		yojimbo::ClientServerConfig* config;
		GameAdapter* adapter;

		Engine::Game* game;
	};
}

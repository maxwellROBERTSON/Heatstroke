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
			GameConfig* config,
			GameAdapter* adapter,
			yojimbo::Address address,
			int maxClients,
			Engine::Game* game
		);
		~GameServer() {}

		// Start the server
		void Start();

		// Update server at a fixed rate, send and recieve packets, process messages from clients, advance server time
		void Update(float);

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

		// Handle a client message for resetting positions
		void HandleClientResetPositions();

		// Function to reset entities to spawn state
		void ResetClientPositions();

		// Reset an entity and its component used by a disconnected client index
		void ResetNetworkEntity(int);

		// Clean up server memory using yojimbo
		void CleanUp();

		// Update network status based on clients and server status
		void UpdateStatus();

		// Get if the server is a listen server
		bool GetListenServer() { return isListenServer; }

		// Set if the server is a listen server and the entity for the server creator
		void SetListenServer(int id) { listenServerEntityId = id; isListenServer = true; }

		// Toggle sendResetMessage for next update loop
		void ReadyToSendResetMessage() override { sendResetPositionsMessage = true; }

		// Get debugging info for the server
		std::map<std::string, std::string> GetInfo();

	private:
		double serverDT = 0;
		int maxClients;
		bool isListenServer = false;
		int listenServerEntityId = -1;

		bool sendResetPositionsMessage = false;

		// Client sends a request for entity data, which moves it into the connection queue
		// Once server, sends the response for entity data, it is removed from the connection queue
		// It is only added to the loadedClients once a ClientInitialized message is recevied,
		// And then it is ready for frequent server update messages
		std::vector<std::pair<int, uint64_t>> connectionQueue;
		std::vector<std::pair<int, uint64_t>> loadedClients;

		yojimbo::Server* server;
		GameConfig* config;
		GameAdapter* adapter;

		Engine::Game* game;
	};
}

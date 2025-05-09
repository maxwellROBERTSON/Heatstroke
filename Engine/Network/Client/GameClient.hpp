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

		// Insecure connection request to server
		void Connect();

		// Update client at a fixed rate, send and recieve packets, process messages from server, advance client time
		void Update();

		// Loop through all server messages, process and release
		void ProcessMessages();

		// Process message type with corresponding function
		void ProcessMessage(yojimbo::Message*);

		// Send game message with any updated entities
		void SendGameUpdate();

		// Handle a response from a request for entity data
		void HandleResponseEntityData(ResponseEntityData*);

		// Send client initialsed message to server
		void SendClientInitialised();

		// Handle a server update message 
		void HandleServerUpdateEntityData(ServerUpdateEntityData*);

		// Clean up client memory using yojimbo
		void CleanUp();

		// Update network status based on client status
		void UpdateStatus();

		// Get debugging info for the client
		std::map<std::string, std::string> GetInfo();

		// Get clientId from yojimbo
		uint64_t GetClientId() { return clientId; }

		// Get clientEntityId
		int GetClientEntityId() { return clientEntityId; }

		// Set clientEntityId
		void SetClientEntityId(int id) { clientEntityId = id; }

		// Toggle sendInitMessage for next update loop
		void ReadyToSendInitMessage() { sendInitMessage = true; }

	private:
		double clientTime;

		uint64_t clientId = 0;

		int clientEntityId = -1;

		bool sendInitMessage = false;

		yojimbo::Address serverAddress;

		yojimbo::Client* client;
		yojimbo::ClientServerConfig* config;
		GameAdapter* adapter;

		Engine::Game* game;
	};
}

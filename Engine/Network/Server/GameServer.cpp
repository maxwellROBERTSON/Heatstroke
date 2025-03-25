#pragma once

#include "GameServer.hpp"

namespace Engine
{
	GameServer::GameServer(
		yojimbo::ClientServerConfig* config,
		GameAdapter* adapter,
		yojimbo::Address address,
		int maxClients,
		Engine::Game* game
	)
		:
		config(config),
		adapter(adapter),
		maxClients(maxClients)
	{
		// Initialise server
		double time = 1.0;
		server = YOJIMBO_NEW(yojimbo::GetDefaultAllocator(), yojimbo::Server,
			yojimbo::GetDefaultAllocator(),
			DEFAULT_PRIVATE_KEY,
			address,
			*config,
			*adapter,
			time);

		adapter->SetServer(server);
		mGame = game;
		Start();
	}

	void GameServer::Start()
	{
		// Start server
		server->Start(maxClients);
	}

	void GameServer::Update()
	{
		// update server and process messages
		server->AdvanceTime(server->GetTime() + dt);
		server->ReceivePackets();
		ProcessMessages();

		// ... process client inputs ...
		// ... update game ...
		// ... send game state to clients ...

		server->SendPackets();
	}

	void GameServer::ProcessMessages()
	{
		for (int i = 0; i < maxClients; i++)
		{
			if (server->IsClientConnected(i))
			{
				for (int j = 0; j < config->numChannels; j++)
				{
					GameMessage* message = (GameMessage*)server->ReceiveMessage(i, j);
					while (message != NULL)
					{
						if (message->GetType() == REQUEST_MESSAGE)
						{
							RequestMessage* derived = dynamic_cast<RequestMessage*>(message);
							HandleRequestMessage(i, static_cast<RequestType>(derived->sequence));
						}
						else
						{
							ProcessMessage(i, message);
						}
						server->ReleaseMessage(i, message);
						message = (GameMessage*)server->ReceiveMessage(i, j);
					}
				}
			}
			else
			{
				server->DisconnectClient(i);
			}
		}
	}

	void GameServer::HandleRequestMessage(int clientIndex, RequestType type)
	{
		yojimbo::Message* message = adapter->factory->CreateMessage(GAME_MESSAGE);
		server->SendServerMessage(clientIndex, yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, message);
	}

	void GameServer::ProcessMessage(int clientIndex, GameMessage* message)
	{
		//std::cout << "Processing message from client " << clientIndex << " with messageID " << message->GetId() << std::endl;
		std::cout << "MESSAGE FROM CLIENT " << clientIndex << " " << message->sequence << " MESSAGEID = " << message->GetId() << std::endl;
	}

	void GameServer::CleanUp()
	{
		// Clean up server
		YOJIMBO_DELETE(yojimbo::GetDefaultAllocator(), GameAdapter, adapter);
		YOJIMBO_DELETE(yojimbo::GetDefaultAllocator(), Server, server);
		ShutdownYojimbo();
	}

	void GameServer::UpdateStatus()
	{
		;
	}

	std::map<std::string, std::string> GameServer::GetInfo()
	{
		std::map<std::string, std::string> info;

		// Server Info
		info["Dt"] = std::to_string(dt);
		info["Max Clients"] = std::to_string(maxClients);
		info["Num Connected Clients"] = std::to_string(server->GetNumConnectedClients());
		const uint8_t* addPtr = server->GetAddress().GetAddress4();
		std::string ipAddress = std::to_string(addPtr[0]) + "." +
			std::to_string(addPtr[1]) + "." +
			std::to_string(addPtr[2]) + "." +
			std::to_string(addPtr[3]);
		info["Address"] = ipAddress;

		return info;
	}
}

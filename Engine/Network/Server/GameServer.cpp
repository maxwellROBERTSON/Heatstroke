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
		maxClients(maxClients),
		game(game)
	{
		// Check max client limit
		if (maxClients > yojimbo::MaxClients)
			game->GetNetwork().Reset();

		// Initialise server
		serverTime = yojimbo_time();
		server = YOJIMBO_NEW(yojimbo::GetDefaultAllocator(), yojimbo::Server,
			yojimbo::GetDefaultAllocator(),
			DEFAULT_PRIVATE_KEY,
			address,
			*config,
			*adapter,
			serverTime);

		adapter->SetServer(server);

		game->GetNetwork().SetStatus(Status::SERVER_INITIALIZED);

		Start();
	}

	// Start the server
	void GameServer::Start()
	{
		server->Start(maxClients);
	}

	// Update server at a fixed rate, send and recieve packets, process messages from clients, advance server time
	void GameServer::Update()
	{
		serverTime = server->GetTime();
		if (serverTime + dt > yojimbo_time()) {
			return;
		}
		serverTime += dt;

		server->SendPackets();

		server->ReceivePackets();

		ProcessMessages();

		server->AdvanceTime(serverTime);

		// ... process client inputs ...
		// ... update game ...
		// ... send game state to clients ...

	}

	// Loop through all client messages, process and release
	void GameServer::ProcessMessages()
	{
		for (int i = 0; i < maxClients; i++)
		{
			if (server->IsClientConnected(i))
			{
				for (int j = 0; j < config->numChannels; j++)
				{
					yojimbo::Message* message = server->ReceiveMessage(i, j);
					while (message != NULL)
					{
						ProcessMessage(i, message);
						server->ReleaseMessage(i, message);
						message = server->ReceiveMessage(i, j);
					}
				}
			}
			else
			{
				server->DisconnectClient(i);
			}
		}
	}

	// Process message type with corresponding function
	void GameServer::ProcessMessage(int clientIndex, yojimbo::Message* message)
	{
		std::cout << "MESSAGE FROM CLIENT " << clientIndex << " WITH: TYPE = " << message->GetType() << ", MESSAGEID = " << message->GetId() << std::endl;
		if (message->GetType() == REQUEST_MESSAGE)
		{
			RequestMessage* derived = (RequestMessage*)message;
			HandleRequestMessage(clientIndex, derived->requestType);
		}
	}

	// Handle request messages, given RequestType send correct data
	void GameServer::HandleRequestMessage(int clientIndex, RequestType requestType)
	{
		RequestResponseMessage* message = static_cast<RequestResponseMessage*>(server->CreateMessage(clientIndex, REQUEST_RESPONSE_MESSAGE));
		if (message)
		{
			message->responseType = static_cast<ResponseType>(requestType);
			if (message->responseType == ResponseType::ENTITY_DATA_RESPONSE)
			{
				int bytes = game->GetEntityManager().GetTotalDataSize();
				uint8_t* block = server->AllocateBlock(clientIndex, bytes);
				if (block)
				{
					game->GetEntityManager().AssignNextClient(server->GetClientId(clientIndex));
					game->GetEntityManager().GetAllData(block);
					server->AttachBlockToMessage(clientIndex, message, block, bytes);
					server->SendServerMessage(clientIndex, yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, message);
					std::cout << "MESSAGE TO CLIENT " << clientIndex << " WITH: TYPE = " << message->GetType() << ", MESSAGEID = ";
					std::cout << message->GetId() << ", BLOCK SIZE = " << bytes << ", RESPONSETYPE = " << static_cast<int>(message->responseType) << std::endl;
					//server->SendPackets();
					//message->DetachBlock();
					//server->FreeBlock(clientIndex, block);
				}
				else
				{
					server->ReleaseMessage(clientIndex, message);
				}
			}
		}
		else
		{
			server->ReleaseMessage(clientIndex, message);
		}
		//adapter->factory->ReleaseMessage(message);
		//server->ReleaseMessage(clientIndex, message);
	}

	// Clean up server memory using yojimbo
	void GameServer::CleanUp()
	{
		// Clean up server
		YOJIMBO_DELETE(yojimbo::GetDefaultAllocator(), GameAdapter, adapter);
		server->Stop();
		YOJIMBO_DELETE(yojimbo::GetDefaultAllocator(), Server, server);
	}

	// Update network status based on clients and server status
	void GameServer::UpdateStatus()
	{
		;
	}

	// Get debugging info for the server
	std::map<std::string, std::string> GameServer::GetInfo()
	{
		std::map<std::string, std::string> info;

		// Server Info
		info["Yojimbo Time"] = std::to_string(yojimbo_time());
		info["Server Time"] = std::to_string(server->GetTime());
		info["Dt"] = std::to_string(dt);
		info["Max Clients"] = std::to_string(maxClients);
		info["Num Connected Clients"] = std::to_string(server->GetNumConnectedClients());
		const uint8_t* addPtr = server->GetAddress().GetAddress4();
		std::string ipAddress = std::to_string(addPtr[0]) + "." +
			std::to_string(addPtr[1]) + "." +
			std::to_string(addPtr[2]) + "." +
			std::to_string(addPtr[3]);
		info["Address"] = ipAddress;
		uint16_t port = server->GetAddress().GetPort();
		info["Port"] = std::to_string(port);

		return info;
	}
}

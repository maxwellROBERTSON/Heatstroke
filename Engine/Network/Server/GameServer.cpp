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

		server->SetLatency(1000.0f);
		server->SetJitter(100.0f);
		server->SetPacketLoss(25.0f);
		server->SetDuplicates(25.0f);

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
		server->AdvanceTime(serverTime);

		server->ReceivePackets();

		ProcessMessages();

		server->SendPackets();

		/*std::cout << "Yojimbo Time: " << yojimbo_time() << std::endl;
		std::cout << "Server Time: " << serverTime << std::endl;*/
		//FreeCurrentBlock();

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
		std::cout << GameMessageTypeStrings[message->GetType()] << " FROM CLIENT " << clientIndex << " WITH MESSAGEID = " << message->GetId() << std::endl;
		if (message->GetType() == REQUEST_ENTITY_DATA)
		{
			HandleRequestEntityData(clientIndex);
		}
		else if (message->GetType() == CLIENT_UPDATE_ENTITY_DATA)
		{
			HandleClientUpdateEntityData(clientIndex, (ClientUpdateEntityData*)message);
		}
	}

	// Send a message receieved message
	void GameServer::HandleSendMessageReceived(int clientIndex)
	{
		/*MessageReceived* message = static_cast<MessageReceived*>(server->CreateMessage(clientIndex, MESSAGE_RECEIVED));
		if (message)
		{
			if (!server->CanSendMessage(clientIndex, yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED))
			{
				std::cout << "MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[MESSAGE_RECEIVED] << " TO CLIENT " << clientIndex << std::endl;
				return;
			}
			server->SendServerMessage(clientIndex, yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, message);
			std::cout << GameMessageTypeStrings[MESSAGE_RECEIVED] << " TO CLIENT " << clientIndex << " WITH: ";
			std::cout << "MESSAGEID = " << message->GetId() << std::endl;
		}
		else
		{
			std::cout << "FAILED TO SEND " << GameMessageTypeStrings[MESSAGE_RECEIVED] << " TO CLIENT " << clientIndex << std::endl;
		}*/
	}

	// Handle a request for entity data
	void GameServer::HandleRequestEntityData(int clientIndex)
	{
		ResponseEntityData* message = static_cast<ResponseEntityData*>(server->CreateMessage(clientIndex, RESPONSE_ENTITY_DATA));
		if (message)
		{
			int bytes = game->GetEntityManager().GetTotalDataSize();
			uint8_t* block = server->AllocateBlock(clientIndex, bytes);
			if (block)
			{
				game->GetEntityManager().AssignNextClient(server->GetClientId(clientIndex));
				game->GetEntityManager().GetAllData(block);
				server->AttachBlockToMessage(clientIndex, message, block, bytes);
				if (!server->CanSendMessage(clientIndex, yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED))
				{
					std::cout << "MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[RESPONSE_ENTITY_DATA] << " TO CLIENT " << clientIndex << std::endl;
					return;
				}
				server->SendServerMessage(clientIndex, yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, message);
				std::cout << GameMessageTypeStrings[RESPONSE_ENTITY_DATA] << " TO CLIENT " << clientIndex << " WITH: ";
				std::cout << "MESSAGEID = " << message->GetId() << ", BLOCK SIZE = " << bytes << std::endl;
				return;
				//server->SendPackets();
				//message->DetachBlock();
				//server->FreeBlock(clientIndex, block);
			}
			else
			{
				server->ReleaseMessage(clientIndex, message);
			}
		}
		std::cout << "FAILED TO SEND " << GameMessageTypeStrings[RESPONSE_ENTITY_DATA] << " TO CLIENT " << clientIndex << std::endl;
		//adapter->factory->ReleaseMessage(message);
		//server->ReleaseMessage(clientIndex, message);
	}

	// Handle a response for entity data
	void GameServer::HandleClientUpdateEntityData(int clientIndex, ClientUpdateEntityData* message)
	{
		if (message->GetBlockSize() == 0)
			throw std::runtime_error("Null block data size");
		HandleSendMessageReceived(clientIndex);
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

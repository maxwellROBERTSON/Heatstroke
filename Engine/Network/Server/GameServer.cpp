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
		server->AdvanceTime(serverTime);

		server->ReceivePackets();

		ProcessMessages();

		QueueStateMessages();

		server->SendPackets();
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
				ResetNetworkEntity(i);
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
			connectionQueue.push_back(std::make_pair(clientIndex, server->GetClientId(clientIndex)));
		}
		else if (message->GetType() == CLIENT_INITIALIZED)
		{
			loadedClients.push_back(std::make_pair(clientIndex, server->GetClientId(clientIndex)));
		}
		else if (message->GetType() == CLIENT_UPDATE_ENTITY_DATA)
		{
			HandleClientUpdateEntityData(clientIndex, (ClientUpdateEntityData*)message);
		}
	}

	// Handle a client update of the entity data
	void GameServer::HandleClientUpdateEntityData(int clientIndex, ClientUpdateEntityData* message)
	{
		int blockSize = message->GetBlockSize();
		if (blockSize == 0)
			throw ("Null block data size");

		if (game->GetNetwork().GetStatus() == Status::SERVER_INITIALIZED)
		{
			if (blockSize != 0)
			{
				if (isListenServer)
				{
					game->GetEntityManager().SetAllChangedData(message->GetBlockData(), listenServerEntityId);
				}
				else
				{
					game->GetEntityManager().SetAllChangedData(message->GetBlockData(), -1);
				}
				std::cout << "Data updated from client " << clientIndex << std::endl;
			}
			else
			{
				std::cout << "FAILED, BLOCK SIZE IS 0." << std::endl;
			}
		}
		else
		{
			std::cout << "FAILED, SERVER NOT YET INITIALISED." << std::endl;
		}
	}

	// Send entity state update to each client, if new client, send all data
	void GameServer::QueueStateMessages()
	{
		for (auto client : connectionQueue)
		{
			HandleRequestEntityData(client.first);
			auto it = std::find(connectionQueue.begin(), connectionQueue.end(), client);
			connectionQueue.erase(it);
		}

		int bytes = game->GetEntityManager().GetTotalChangedDataSize();
		if (bytes != 0)
		{
			for (auto client : loadedClients)
			{
				ServerUpdateEntityData* message = static_cast<ServerUpdateEntityData*>(server->CreateMessage(client.first, SERVER_UPDATE_ENTITY_DATA));
				if (message->GetId() > 20)
					exit(0);
				if (message)
				{
					uint8_t* block = server->AllocateBlock(client.first, 1024);
					std::cout << "Block allocated at: " << static_cast<void*>(block) << std::endl;
					if (block)
					{
						game->GetEntityManager().GetAllChangedData(block);
						server->AttachBlockToMessage(client.first, message, block, 1024);
						if (!server->CanSendMessage(client.first, yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED))
						{
							std::cout << "MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[SERVER_UPDATE_ENTITY_DATA] << std::endl;
							server->ReleaseMessage(client.first, message);
							continue;
						}
						server->SendServerMessage(client.first, yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED, message);
						std::cout << GameMessageTypeStrings[SERVER_UPDATE_ENTITY_DATA] << " TO CLIENT " << client.first << " WITH ";
						std::cout << "MESSAGEID = " << message->GetId() << ", BLOCK SIZE = " << 1024 << std::endl;
						continue;
					}
					else
					{
						std::cout << "Block allocation failed." << std::endl;
						server->ReleaseMessage(client.first, message);
					}
				}
				std::cout << "FAILED TO SEND " << GameMessageTypeStrings[SERVER_UPDATE_ENTITY_DATA] << " TO CLIENT " << client.first << std::endl;
			}
			game->GetEntityManager().ResetChanged();
		}
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
				game->GetEntityManager().AssignNextClient(server->GetClientId(clientIndex), isListenServer);
				game->GetEntityManager().GetAllData(block);
				server->AttachBlockToMessage(clientIndex, message, block, bytes);
				if (!server->CanSendMessage(clientIndex, yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED))
				{
					std::cout << "MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[RESPONSE_ENTITY_DATA] << " TO CLIENT " << clientIndex << std::endl;
					return;
				}
				server->SendServerMessage(clientIndex, yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, message);
				std::cout << GameMessageTypeStrings[RESPONSE_ENTITY_DATA] << " TO CLIENT " << clientIndex << " WITH ";
				std::cout << "MESSAGEID = " << message->GetId() << ", BLOCK SIZE = " << bytes << std::endl;
				return;
			}
			else
			{
				server->ReleaseMessage(clientIndex, message);
			}
		}
		std::cout << "FAILED TO SEND " << GameMessageTypeStrings[RESPONSE_ENTITY_DATA] << " TO CLIENT " << clientIndex << std::endl;
	}

	// Reset an entity and its component used by a disconnected client
	void GameServer::ResetNetworkEntity(int clientIndex)
	{
		uint64_t clientId = -1;
		for (int j = 0; j < connectionQueue.size(); j++)
		{
			if (connectionQueue[j].first == clientIndex)
			{
				clientId = connectionQueue[j].second;
				connectionQueue.erase(connectionQueue.begin() + j);
				break;
			}
		}
		if (clientId == -1)
		{
			for (int j = 0; j < loadedClients.size(); j++)
			{
				if (loadedClients[j].first == clientIndex)
				{
					clientId = loadedClients[j].second;
					loadedClients.erase(loadedClients.begin() + j);
					break;
				}
			}
		}
		if (clientId == -1) return;

		std::vector<int> entityWithNetwork = game->GetEntityManager().GetEntitiesWithComponent(NETWORK);
		NetworkComponent* networkComp;
		for (int entity : entityWithNetwork)
		{
			networkComp = reinterpret_cast<NetworkComponent*>(game->GetEntityManager().GetComponentOfEntity(entity, NETWORK));
			if (networkComp->GetClientId() == clientId)
			{
				networkComp->SetClientId(-1);
				RenderComponent* renderComp = reinterpret_cast<RenderComponent*>(game->GetEntityManager().GetComponentOfEntity(entity, RENDER));
				renderComp->SetIsActive(false);
				Entity* entityPtr = game->GetEntityManager().GetEntity(entity);
				entityPtr->ResetToSpawnState();
				game->GetEntityManager().AddToNetworkComponentQueue(entityPtr->GetComponent(NETWORK));
				if (isListenServer)
				{
					PhysicsComponent* physicsComponent = reinterpret_cast<Engine::PhysicsComponent*>(game->GetEntityManager().GetComponentOfEntity(entity, PHYSICS));
					physicsComponent->SetSimulation(PhysicsComponent::PhysicsSimulation::NOTUPDATED);
					game->GetEntityManager().AddUpdatedPhysicsComp(physicsComponent, false);
				}
			}
		}
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

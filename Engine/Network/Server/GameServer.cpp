#include "GameServer.hpp"

#include "../../Core/Log.hpp"

namespace Engine
{
	GameServer::GameServer(
		GameConfig* config,
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
		int dtCount = 0;
		while(serverTime + dtCount * dt < yojimbo_time())
		{
			dtCount++;
		}
		if (dtCount == 0)
		{
			return;
		}
		else
		{
			serverTime += dtCount * dt;
		}
		server->AdvanceTime(serverTime);

		server->ReceivePackets();

		EntityManager& manager = game->GetEntityManager();

		ProcessMessages();

		if (sendResetPositionsMessage)
		{
			HandleClientResetPositions();
			manager.SetResetTimer(5.f);
			sendResetPositionsMessage = false;
		}

		if (manager.GetResetTimer() != 0.f)
		{
			ResetClientPositions();
			manager.DecreaseResetTimer(dtCount * dt);
		}
		else
		{
			QueueStateMessages();
		}

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
		DLOG(GameMessageTypeStrings[message->GetType()] << " FROM CLIENT " << clientIndex << " WITH MESSAGEID = " << message->GetId());
		if (message->GetType() == REQUEST_ENTITY_DATA)
		{
			connectionQueue.push_back(std::make_pair(clientIndex, server->GetClientId(clientIndex)));
		}
		else if (message->GetType() == CLIENT_INITIALIZED)
		{
			loadedClients.push_back(std::make_pair(clientIndex, server->GetClientId(clientIndex)));
			sendResetPositionsMessage = true;
		}
		else if (message->GetType() == CLIENT_UPDATE_ENTITY_DATA)
		{
			if (game->GetEntityManager().GetResetTimer() == 0.f)
				HandleClientUpdateEntityData(clientIndex, (ClientUpdateEntityData*)message);
		}
		else if (message->GetType() == CLIENT_RESET_POSITIONS)
		{
			sendResetPositionsMessage = true;
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
				DLOG("Data updated from client " << clientIndex);
			}
			else
			{
				DLOG("FAILED, BLOCK SIZE IS 0.");
			}
		}
		else
		{
			DLOG("FAILED, SERVER NOT YET INITIALISED.");
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

		size_t bytes = game->GetEntityManager().GetTotalChangedDataSize();
		if (bytes != 0)
		{
			for (auto client : loadedClients)
			{
				ServerUpdateEntityData* message = static_cast<ServerUpdateEntityData*>(server->CreateMessage(client.first, SERVER_UPDATE_ENTITY_DATA));
				if (message)
				{
					uint8_t* block = server->AllocateBlock(client.first, 1024);
					DLOG("Block allocated at: " << static_cast<void*>(block));
					if (block)
					{
						game->GetEntityManager().GetAllChangedData(block);
						server->AttachBlockToMessage(client.first, message, block, 1024);
						if (!server->CanSendMessage(client.first, yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED))
						{
							DLOG("MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[SERVER_UPDATE_ENTITY_DATA]);
							server->ReleaseMessage(client.first, message);
							continue;
						}
						server->SendServerMessage(client.first, yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED, message);
						DLOG(GameMessageTypeStrings[SERVER_UPDATE_ENTITY_DATA] << " TO CLIENT " << client.first << " WITH MESSAGEID = " << message->GetId() << ", BLOCK SIZE = " << 1024);
						continue;
					}
					else
					{
						DLOG("Block allocation failed.");
						server->ReleaseMessage(client.first, message);
					}
				}
				DLOG("FAILED TO SEND " << GameMessageTypeStrings[SERVER_UPDATE_ENTITY_DATA] << " TO CLIENT " << client.first);
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
					DLOG("MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[RESPONSE_ENTITY_DATA] << " TO CLIENT " << clientIndex);
					return;
				}
				server->SendServerMessage(clientIndex, yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, message);
				DLOG(GameMessageTypeStrings[RESPONSE_ENTITY_DATA] << " TO CLIENT " << clientIndex << " WITH MESSAGEID = " << message->GetId() << ", BLOCK SIZE = " << bytes);
				return;
			}
			else
			{
				server->ReleaseMessage(clientIndex, message);
			}
		}
		DLOG("FAILED TO SEND " << GameMessageTypeStrings[RESPONSE_ENTITY_DATA] << " TO CLIENT " << clientIndex);
	}

	// Handle a client message for resetting positions
	void GameServer::HandleClientResetPositions()
	{
		ResetClientPositions();

		for (int i = 0; i < server->GetNumConnectedClients(); i++)
		{
			ServerResetPositions* message = static_cast<ServerResetPositions*>(server->CreateMessage(i, SERVER_RESET_POSITIONS));
			if (message)
			{
				if (!server->CanSendMessage(i, yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED))
				{
					DLOG("MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[SERVER_RESET_POSITIONS] << " TO CLIENT " << i);
					continue;
				}
				server->SendServerMessage(i, yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, message);
				DLOG(GameMessageTypeStrings[SERVER_RESET_POSITIONS] << " TO CLIENT " << i << " WITH MESSAGEID = " << message->GetId());
				continue;
			}
			DLOG("FAILED TO SEND " << GameMessageTypeStrings[SERVER_RESET_POSITIONS] << " TO CLIENT " << i);
		}
	}

	// Function to reset entities to spawn state
	void GameServer::ResetClientPositions()
	{
		if (isListenServer)
		{
			EntityManager& manager = game->GetEntityManager();
			std::vector<int> entitiesWithPhysicsComponent = manager.GetEntitiesWithComponent(PHYSICS);
			for (int i = 0; i < entitiesWithPhysicsComponent.size(); i++)
			{
				Entity* entity = game->GetEntityManager().GetEntity(entitiesWithPhysicsComponent[i]);
				Engine::PhysicsComponent* physicsComponent = reinterpret_cast<Engine::PhysicsComponent*>(manager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
				if (physicsComponent->GetPhysicsType() == PhysicsComponent::PhysicsType::CONTROLLER)
				{
					entity->ResetToSpawnState();
				}
				if (entity->GetEntityId() == listenServerEntityId)
				{
					glm::vec3 pos = entity->GetPosition();
					physicsComponent->GetController()->setFootPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
				}
			}
		}
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

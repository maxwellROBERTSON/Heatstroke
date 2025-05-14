#include "GameClient.hpp"

namespace Engine
{
	GameClient::GameClient(
		yojimbo::ClientServerConfig* config,
		GameAdapter* adapter,
		yojimbo::Address serverAddress,
		Engine::Game* game)
		:
		config(config),
		adapter(adapter),
		serverAddress(serverAddress),
		game(game)
	{
		// Initialise  client
		clientTime = yojimbo_time();
		client = YOJIMBO_NEW(yojimbo::GetDefaultAllocator(), yojimbo::Client,
			yojimbo::GetDefaultAllocator(),
			yojimbo::Address("0.0.0.0"),
			*config,
			*adapter,
			clientTime);

		while(clientId == 0)
			yojimbo_random_bytes((uint8_t*)&clientId, 8);

		game->GetNetwork().SetStatus(Status::CLIENT_INITIALIZED);

		Connect();
	}

	// Insecure connection request to server
	void GameClient::Connect()
	{
		client->InsecureConnect(DEFAULT_PRIVATE_KEY, clientId, serverAddress);
	}

	// Update client at a fixed rate, send and recieve packets, process messages from server, advance client time
	void GameClient::Update()
	{
		// Check client connection
		yojimbo::ClientState state = client->GetClientState();
		if (state == yojimbo::ClientState::CLIENT_STATE_DISCONNECTED)
		{
			game->GetNetwork().SetStatus(Status::CLIENT_DISCONNECTED);
			std::cout << "Status set to client disconnected." << std::endl;
		}
		else if (state == yojimbo::ClientState::CLIENT_STATE_ERROR)
		{
			game->GetNetwork().SetStatus(Status::CLIENT_CONNECTION_FAILED);
			std::cout << "Status set to client connection failed." << std::endl;
		}

		// Update time against yojimbo time
		clientTime = client->GetTime();
		double t = yojimbo_time();
		if (clientTime + dt > yojimbo_time()) {
			return;
		}
		clientTime += dt;

		clientTime += dt;
		client->AdvanceTime(clientTime);

		client->ReceivePackets();

		UpdateStatus();

		if (client->IsConnected())
		{
			ProcessMessages();

			if (toDoReset)
			{
				HandleServerResetPositions();
				toDoReset = false;
			}

			if (sendInitMessage)
			{
				SendClientInitialised();
				sendInitMessage = false;
			}
			if (sendResetPositionsMessage)
			{
				SendResetPositionsMessage();
				sendResetPositionsMessage = false;
			}
			else if (game->GetEntityManager().HasSceneChanged() && !client->HasMessagesToSend(yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED))
			{
				SendGameUpdate();
			}

			// ... do connected stuff ...
		}

		client->SendPackets();
	}

	// Loop through all server messages, process and release
	void GameClient::ProcessMessages()
	{
		for (int i = 0; i < config->numChannels; i++)
		{
			yojimbo::Message* message = client->ReceiveMessage(i);
			while (message != NULL)
			{
				ProcessMessage(message);
				client->ReleaseMessage(message);
				message = client->ReceiveMessage(i);
			}
		}
	}

	// Process message type with corresponding function
	void GameClient::ProcessMessage(yojimbo::Message* message)
	{
		std::cout << GameMessageTypeStrings[message->GetType()] << " FROM SERVER WITH MESSAGEID = " << message->GetId() << std::endl;
		if (message->GetType() == RESPONSE_ENTITY_DATA)
		{
			HandleResponseEntityData((ResponseEntityData*)message);
		}
		else if (message->GetType() == SERVER_UPDATE_ENTITY_DATA)
		{
			HandleServerUpdateEntityData((ServerUpdateEntityData*)message);
		}
		else if (message->GetType() == SERVER_RESET_POSITIONS)
		{
			toDoReset = true;
		}
	}

	// Send game message with any updated entities
	void GameClient::SendGameUpdate()
	{
		if (game->GetNetwork().GetStatus() != Status::CLIENT_ACTIVE)
			game->GetNetwork().SetStatus(Status::CLIENT_ACTIVE);
		size_t bytes = game->GetEntityManager().GetTotalChangedDataSize();
		if (bytes == 0)
			return;
		ClientUpdateEntityData* message = static_cast<ClientUpdateEntityData*>(client->CreateMessage(CLIENT_UPDATE_ENTITY_DATA));
		if (message)
		{
			uint8_t* block = client->AllocateBlock(bytes);
			std::cout << "Block allocated at: " << static_cast<void*>(block) << std::endl;
			if (block)
			{
				game->GetEntityManager().GetAllChangedData(block);
				client->AttachBlockToMessage(message, block, bytes);
				if (!client->CanSendMessage(yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED))
				{
					std::cout << "MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[CLIENT_UPDATE_ENTITY_DATA] << std::endl;
					client->ReleaseMessage(message);
					return;
				}
				client->SendClientMessage(yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED, message);
				std::cout << GameMessageTypeStrings[CLIENT_UPDATE_ENTITY_DATA] << " TO SERVER WITH ";
				std::cout << "MESSAGEID = " << message->GetId() << ", BLOCK SIZE = " << bytes << std::endl;
				game->GetEntityManager().ResetChanged();
				return; 
			}
			else
			{
				std::cout << "Block allocation failed." << std::endl;
				client->ReleaseMessage(message);
			}
		}
		std::cout << "FAILED TO SEND " << GameMessageTypeStrings[CLIENT_UPDATE_ENTITY_DATA] << " TO SERVER" << std::endl;
	}

	// Handle a response from a request for entity data
	void GameClient::HandleResponseEntityData(ResponseEntityData* message)
	{
		int blockSize = message->GetBlockSize();

		if (game->GetNetwork().GetStatus() != Status::CLIENT_ACTIVE)
		{
			if (blockSize != 0)
			{
				game->GetEntityManager().SetAllData(message->GetBlockData());
				game->GetEntityManager().ResetChanged();
				game->GetNetwork().SetStatus(Status::CLIENT_INITIALIZING_DATA);
			}
			else
			{
				std::cout << "FAILED, BLOCK SIZE IS 0." << std::endl;
			}
		}
		else
		{
			std::cout << "FAILED, CLIENT HAS ALREADY LOADED DATA." << std::endl;
		}
	}

	// Send client initialsed message to server
	void GameClient::SendClientInitialised()
	{
		ClientInitialized* initMessage = static_cast<ClientInitialized*>(client->CreateMessage(CLIENT_INITIALIZED));
		if (initMessage)
		{
			if (!client->CanSendMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED))
			{
				std::cout << "MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[CLIENT_INITIALIZED] << std::endl;
				client->ReleaseMessage(initMessage);
				return;
			}
			client->SendClientMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, initMessage);
			std::cout << GameMessageTypeStrings[CLIENT_INITIALIZED] << " TO SERVER WITH ";
			std::cout << "MESSAGEID = " << initMessage->GetId() << std::endl;
			return;
		}
		std::cout << "FAILED TO SEND " << GameMessageTypeStrings[CLIENT_INITIALIZED] << " TO SERVER" << std::endl;
	}

	// Send reset message to server
	void GameClient::SendResetPositionsMessage()
	{
		ClientResetPositions* resetMessage = static_cast<ClientResetPositions*>(client->CreateMessage(CLIENT_RESET_POSITIONS));
		if (resetMessage)
		{
			if (!client->CanSendMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED))
			{
				std::cout << "MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[CLIENT_RESET_POSITIONS] << std::endl;
				client->ReleaseMessage(resetMessage);
				return;
			}
			client->SendClientMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, resetMessage);
			std::cout << GameMessageTypeStrings[CLIENT_RESET_POSITIONS] << " TO SERVER WITH ";
			std::cout << "MESSAGEID = " << resetMessage->GetId() << std::endl;
			return;
		}
		std::cout << "FAILED TO SEND " << GameMessageTypeStrings[CLIENT_RESET_POSITIONS] << " TO SERVER" << std::endl;
	}

	// Handle a server update message
	void GameClient::HandleServerUpdateEntityData(ServerUpdateEntityData* message)
	{
		int blockSize = message->GetBlockSize();

		if (game->GetNetwork().GetStatus() == Status::CLIENT_ACTIVE)
		{
			if (blockSize != 0)
			{
				game->GetEntityManager().SetAllChangedData(message->GetBlockData(), clientEntityId);
				game->GetEntityManager().ResetChanged();
				std::cout << "Data updated from server" << std::endl;
			}
			else
			{
				std::cout << "FAILED, BLOCK SIZE IS 0." << std::endl;
			}
		}
		else
		{
			std::cout << "FAILED, CLIENT NOT YET ACTIVE." << std::endl;
		}
	}

	// Handle a server message for resetting positions
	void GameClient::HandleServerResetPositions()
	{
		std::cout << "HandleServerResetPositions()\n";
		if (game->GetNetwork().GetStatus() == Status::CLIENT_ACTIVE)
		{
			std::cout << "HandleServerResetPositions() CLIENT_ACTIVE\n";
			EntityManager& manager = game->GetEntityManager();
			std::vector<int> entitiesWithNetworkComponent = manager.GetEntitiesWithComponent(NETWORK);
			std::cout << "entitiesWithNetworkComponent.size() " << entitiesWithNetworkComponent.size() << std::endl;
			for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
			{
				Entity* entity = game->GetEntityManager().GetEntity(entitiesWithNetworkComponent[i]);
				NetworkComponent* networkComponent = reinterpret_cast<Engine::NetworkComponent*>(manager.GetComponentOfEntity(entity->GetEntityId(), NETWORK));
				std::cout << "Entity id: " << entity->GetEntityId() << " netcompclientId: " << networkComponent->GetClientId() << " clientId: " << clientId << std::endl;
				if (networkComponent->GetClientId() == clientId)
				{
					Engine::PhysicsComponent* physicsComponent = reinterpret_cast<Engine::PhysicsComponent*>(manager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
					glm::vec3 pos = entity->GetPosition();
					physicsComponent->GetController()->setFootPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
					std::cout << "Set foot pos: " << pos.x << " " << pos.y << " " << pos.z << std::endl;
				}
			}
		}
		else
		{
			std::cout << "FAILED, CLIENT NOT YET ACTIVE." << std::endl;
		}
	}

	// Clean up client memory using yojimbo
	void GameClient::CleanUp()
	{
		// Clean up client
		YOJIMBO_DELETE(yojimbo::GetDefaultAllocator(), GameAdapter, adapter);
		Status status = game->GetNetwork().GetStatus();
		if (status == Status::CLIENT_CONNECTED ||
			status == Status::CLIENT_LOADING_DATA ||
			status == Status::CLIENT_INITIALIZING_DATA ||
			status == Status::CLIENT_ACTIVE
			)
			client->Disconnect();
		YOJIMBO_DELETE(yojimbo::GetDefaultAllocator(), Client, client);
	}

	// Update network status based on client status
	void GameClient::UpdateStatus()
	{
		Status status = game->GetNetwork().GetStatus();
		if (client->IsConnecting())
		{
			if (status != Status::CLIENT_CONNECTING)
				game->GetNetwork().SetStatus(Status::CLIENT_CONNECTING);
		}
		else if (client->IsConnected())
		{
			if (status == Status::CLIENT_LOADING_DATA || status == Status::CLIENT_INITIALIZING_DATA || status == Status::CLIENT_ACTIVE)
				return;
			clientTime = yojimbo_time();
			RequestEntityData* message = static_cast<RequestEntityData*>(client->CreateMessage(REQUEST_ENTITY_DATA));
			if (!client->CanSendMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED))
			{
				std::cout << "MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[REQUEST_ENTITY_DATA] << std::endl;
				return;
			}
			client->SendClientMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, message);
			std::cout << GameMessageTypeStrings[REQUEST_ENTITY_DATA] << " TO SERVER WITH ";
			std::cout << "MESSAGEID = " << message->GetId() << std::endl;
			game->GetEntityManager().ResetChanged();
			game->GetNetwork().SetStatus(Status::CLIENT_LOADING_DATA);
		}
		else if (client->IsDisconnected())
		{
			if (status != Status::CLIENT_DISCONNECTED)
				game->GetNetwork().SetStatus(Status::CLIENT_DISCONNECTED);
			// EDITED
		}
		else if (client->ConnectionFailed())
		{
			if (status != Status::CLIENT_CONNECTION_FAILED)
				game->GetNetwork().SetStatus(Status::CLIENT_CONNECTION_FAILED);
		}
	}

	// Get debugging info for the client
	std::map<std::string, std::string> GameClient::GetInfo()
	{
		std::map<std::string, std::string> info;

		// Client Info
		info["Yojimbo Time"] = std::to_string(yojimbo_time());
		info["Client Time"] = std::to_string(clientTime);
		info["Dt"] = std::to_string(dt);
		info["Client Address"] = std::to_string(*client->GetAddress().GetAddress4());
		const uint8_t* addPtr = client->GetAddress().GetAddress4();
		std::string ipAddress = std::to_string(addPtr[0]) + "." +
			std::to_string(addPtr[1]) + "." +
			std::to_string(addPtr[2]) + "." +
			std::to_string(addPtr[3]);
		info["Client Address"] = ipAddress;
		if (client->IsConnected())
		{
			info["Client Connected"] = "true";
			addPtr = serverAddress.GetAddress4();
			ipAddress = std::to_string(addPtr[0]) + "." +
				std::to_string(addPtr[1]) + "." +
				std::to_string(addPtr[2]) + "." +
				std::to_string(addPtr[3]);
			info["Server Address"] = ipAddress;
		}
		else
		{
			info["Client Connected"] = "false";
		}
		info["Client Index"] = std::to_string(client->GetClientIndex());
		info["Client Id"] = std::to_string(client->GetClientId());
		if (client->IsConnected())
		{
			info["Is Loopback"] = std::to_string(client->IsLoopback());
		}
		else
		{
			info["Is Loopback"] = "false";
		}

		return info;
	}
}

#include "GameClient.hpp"

#include "../../Core/Log.hpp"

namespace Engine
{
	GameClient::GameClient(
		GameConfig* config,
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
		client = YOJIMBO_NEW(yojimbo::GetDefaultAllocator(), yojimbo::Client,
			yojimbo::GetDefaultAllocator(),
			yojimbo::Address("0.0.0.0"),
			*config,
			*adapter,
			yojimbo_time());

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
	void GameClient::Update(float timeDelta)
	{
		// Check client connection
		yojimbo::ClientState state = client->GetClientState();
		if (state == yojimbo::ClientState::CLIENT_STATE_DISCONNECTED)
		{
			game->GetNetwork().SetStatus(Status::CLIENT_DISCONNECTED);
			DLOG("Status set to client disconnected.");
		}
		else if (state == yojimbo::ClientState::CLIENT_STATE_ERROR)
		{
			game->GetNetwork().SetStatus(Status::CLIENT_CONNECTION_FAILED);
			DLOG("Status set to client connection failed.");
		}

		clientDT += timeDelta;

		if (clientDT < dt)
		{
			return;
		}

		clientDT -= dt;

		client->AdvanceTime(client->GetTime() + dt);

		client->ReceivePackets();

		EntityManager& manager = game->GetEntityManager();

		UpdateStatus();

		if (client->IsConnected())
		{
			ProcessMessages();

			if (manager.GetResetTimer() != 0.f)
			{
				if (manager.GetToBeReset())
				{
					HandleServerResetPositions();
					manager.SetToBeReset(false);
				}
				manager.DecreaseResetTimer(dt);
			}
			else
			{
				if (sendInitMessage)
				{
					SendClientInitialised();
					sendInitMessage = false;
				}
				else if (sendResetPositionsMessage)
				{
					SendResetPositionsMessage();
					sendResetPositionsMessage = false;
				}
				else if (manager.HasSceneChanged() && !client->HasMessagesToSend(yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED))
				{
					SendGameUpdate();
				}
			}
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
		DLOG(GameMessageTypeStrings[message->GetType()] << " FROM SERVER WITH MESSAGEID = " << message->GetId());
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
			yojimbo::NetworkInfo info;
			client->GetNetworkInfo(info);
			game->GetEntityManager().SetResetTimer(5.f - info.RTT / 2000);
		}
	}

	// Send game message with any updated entities
	void GameClient::SendGameUpdate()
	{
		if (game->GetNetwork().GetStatus() != Status::CLIENT_ACTIVE)
			game->GetNetwork().SetStatus(Status::CLIENT_ACTIVE);
		int bytes = game->GetEntityManager().GetTotalChangedDataSize();
		if (bytes == 0 || bytes > this->config->channel[yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED].maxBlockSize)
			return;
		ClientUpdateEntityData* message = static_cast<ClientUpdateEntityData*>(client->CreateMessage(CLIENT_UPDATE_ENTITY_DATA));
		if (message)
		{
			bytes = (bytes + 7) & ~7;
			uint8_t* block = client->AllocateBlock(bytes);
			DLOG("Block allocated at: " << static_cast<void*>(block));
			if (block)
			{
				game->GetEntityManager().GetAllChangedData(block);
				client->AttachBlockToMessage(message, block, bytes);
				if (!client->CanSendMessage(yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED))
				{
					DLOG("MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[CLIENT_UPDATE_ENTITY_DATA]);
					client->ReleaseMessage(message);
					return;
				}
				client->SendClientMessage(yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED, message);
				DLOG(GameMessageTypeStrings[CLIENT_UPDATE_ENTITY_DATA] << " TO SERVER WITH MESSAGEID = " << message->GetId() << ", BLOCK SIZE = " << bytes);
				game->GetEntityManager().ResetChanged();
				return; 
			}
			else
			{
				DLOG("Block allocation failed.");
				client->ReleaseMessage(message);
			}
		}
		DLOG("FAILED TO SEND " << GameMessageTypeStrings[CLIENT_UPDATE_ENTITY_DATA] << " TO SERVER");
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
				DLOG("FAILED, BLOCK SIZE IS 0.");
			}
		}
		else
		{
			DLOG("FAILED, CLIENT HAS ALREADY LOADED DATA.");
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
				DLOG("MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[CLIENT_INITIALIZED]);
				client->ReleaseMessage(initMessage);
				return;
			}
			client->SendClientMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, initMessage);
			DLOG(GameMessageTypeStrings[CLIENT_INITIALIZED] << " TO SERVER WITH MESSAGEID = " << initMessage->GetId());
			return;
		}
		DLOG("FAILED TO SEND " << GameMessageTypeStrings[CLIENT_INITIALIZED] << " TO SERVER");
	}

	// Send reset message to server
	void GameClient::SendResetPositionsMessage()
	{
		ClientResetPositions* resetMessage = static_cast<ClientResetPositions*>(client->CreateMessage(CLIENT_RESET_POSITIONS));
		if (resetMessage)
		{
			if (!client->CanSendMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED))
			{
				DLOG("MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[CLIENT_RESET_POSITIONS]);
				client->ReleaseMessage(resetMessage);
				return;
			}
			client->SendClientMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, resetMessage);
			DLOG(GameMessageTypeStrings[CLIENT_RESET_POSITIONS] << " TO SERVER WITH MESSAGEID = " << resetMessage->GetId());
			return;
		}
		DLOG("FAILED TO SEND " << GameMessageTypeStrings[CLIENT_RESET_POSITIONS] << " TO SERVER");
	}

	// Handle a server update message
	void GameClient::HandleServerUpdateEntityData(ServerUpdateEntityData* message)
	{
		int blockSize = message->GetBlockSize();
		DLOG("Block size received of: " << blockSize);

		if (game->GetNetwork().GetStatus() == Status::CLIENT_ACTIVE)
		{
			if (blockSize != 0)
			{
				game->GetEntityManager().SetAllChangedData(message->GetBlockData(), clientEntityId);
				game->GetEntityManager().ResetChanged();
				DLOG("Data updated from server");
			}
			else
			{
				DLOG("FAILED, BLOCK SIZE IS 0.");
			}
		}
		else
		{
			DLOG("FAILED, CLIENT NOT YET ACTIVE.");
		}
	}

	// Handle a server message for resetting positions
	void GameClient::HandleServerResetPositions()
	{
		if (game->GetNetwork().GetStatus() == Status::CLIENT_ACTIVE)
		{
			EntityManager& manager = game->GetEntityManager();
			std::vector<int> entitiesWithNetworkComponent = manager.GetEntitiesWithComponent(NETWORK);
			for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
			{
				Entity* entity = game->GetEntityManager().GetEntity(entitiesWithNetworkComponent[i]);
				entity->ResetToSpawnState();
				NetworkComponent* networkComponent = reinterpret_cast<Engine::NetworkComponent*>(manager.GetComponentOfEntity(entity->GetEntityId(), NETWORK));
				if (networkComponent->GetClientId() == clientId)
				{
					Engine::PhysicsComponent* physicsComponent = reinterpret_cast<Engine::PhysicsComponent*>(manager.GetComponentOfEntity(entity->GetEntityId(), PHYSICS));
					glm::vec3 pos = entity->GetPosition();
					physicsComponent->GetController()->setFootPosition(PxExtendedVec3(pos.x, pos.y, pos.z));
				}
			}
		}
		else
		{
			DLOG("FAILED, CLIENT NOT YET ACTIVE.");
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
			client->AdvanceTime(yojimbo_time());
			RequestEntityData* message = static_cast<RequestEntityData*>(client->CreateMessage(REQUEST_ENTITY_DATA));
			if (!client->CanSendMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED))
			{
				DLOG("MESSAGE QUEUE NOT AVAILABLE FOR " << GameMessageTypeStrings[REQUEST_ENTITY_DATA]);
				return;
			}
			client->SendClientMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, message);
			DLOG(GameMessageTypeStrings[REQUEST_ENTITY_DATA] << " TO SERVER WITH MESSAGEID = " << message->GetId());
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
		info["Client Time"] = std::to_string(client->GetTime());
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

		yojimbo::NetworkInfo netInfo;
		client->GetNetworkInfo(netInfo);

		info["RTT"] = std::to_string(netInfo.RTT);
		info["Min RTT"] = std::to_string(netInfo.minRTT);
		info["Max RTT"] = std::to_string(netInfo.maxRTT);
		info["Avg RTT"] = std::to_string(netInfo.averageRTT);
		info["Avg Jitter"] = std::to_string(netInfo.averageJitter);
		info["Max Jitter"] = std::to_string(netInfo.maxJitter);
		info["Stddev Jitter"] = std::to_string(netInfo.stddevJitter);
		info["Packet Loss (%)"] = std::to_string(netInfo.packetLoss);
		info["Sent Bandwidth (kbps)"] = std::to_string(netInfo.sentBandwidth);
		info["Received Bandwidth (kbps)"] = std::to_string(netInfo.receivedBandwidth);
		info["Acked Bandwidth (kbps)"] = std::to_string(netInfo.ackedBandwidth);
		info["Packets Sent"] = std::to_string(netInfo.numPacketsSent);
		info["Packets Received"] = std::to_string(netInfo.numPacketsReceived);
		info["Packets Acked"] = std::to_string(netInfo.numPacketsAcked);

		return info;
	}
}

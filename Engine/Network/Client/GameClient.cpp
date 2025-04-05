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
		}
		else if (state == yojimbo::ClientState::CLIENT_STATE_ERROR)
		{
			game->GetNetwork().SetStatus(Status::CLIENT_CONNECTION_FAILED);
		}

		// Update time against yojimbo time
		clientTime = client->GetTime();
		if (clientTime + dt > yojimbo_time()) {
			clientTime += dt;
		}
		clientTime += dt;

		std::cout << "sending packets" << std::endl;
		client->SendPackets();

		std::cout << "receiving packets" << std::endl;
		client->ReceivePackets();

		UpdateStatus();

		if (client->IsConnected())
		{
			ProcessMessages();

			// ... do connected stuff ...

			// send a message when space is pressed
		}
		/*if (client->HasMessagesToSend(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED))
		{
			std::cout << "MESSAGE TO SEND" << std::endl;
		}*/

		std::cout << "advancing to time: " << clientTime << std::endl;
		client->AdvanceTime(clientTime);
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
		if (message->GetType() == REQUEST_RESPONSE_MESSAGE)
		{
			HandleResponseMessage((RequestResponseMessage*)message);
		}
		std::cout << "Processing message from server with messageID " << message->GetId() << std::endl;
	}

	// Handle a response from a request message, given ResponseType set correct data
	void GameClient::HandleResponseMessage(RequestResponseMessage* message)
	{
		std::cout << "RESPONSE MESSAGE FROM SERVER WITH: TYPE = " << message->GetType() << ", MESSAGEID = ";
		std::cout << message->GetId() << ", BLOCK SIZE = " << message->GetBlockSize() << ", RESPONSETYPE = " << static_cast<int>(message->responseType) << std::endl;
		int size = message->GetBlockSize();

		if (message->responseType == ResponseType::ENTITY_DATA_RESPONSE && game->GetNetwork().GetStatus() != Status::CLIENT_LOADED)
		{
			if (message->GetBlockSize() == 0)
				throw std::runtime_error("Null block data size");
			game->GetEntityManager().SetAllData(message->GetBlockData());
			EntityManager* manager = &(game->GetEntityManager());
			std::vector<int> vec = manager->GetEntitiesWithComponent(PHYSICS);
			PhysicsComponent* comp;
			for (int i = 0; i < vec.size(); i++)
			{
				comp = reinterpret_cast<PhysicsComponent*>(manager->GetComponentOfEntity(vec[i], PHYSICS));
				glm::mat4 mat = manager->GetEntity(vec[i])->GetModelMatrix();
				PhysicsComponent::PhysicsType type = comp->GetPhysicsType();
				if (type == PhysicsComponent::PhysicsType::STATIC)
				{
					Engine::vk::Model& model = game->GetModels()[vec[i]];
					comp->InitComplexShape(game->GetPhysicsWorld(), type, model, mat, vec[i]);
				}
				else
				{
					comp->Init(game->GetPhysicsWorld(), type, mat, vec[i]);
				}
			}
			std::vector<int> entitiesWithNetworkComponent = manager->GetEntitiesWithComponent(NETWORK);
			NetworkComponent* networkComponent;
			for (int i = 0; i < entitiesWithNetworkComponent.size(); i++)
			{
				networkComponent = reinterpret_cast<NetworkComponent*>(manager->GetComponentOfEntity(entitiesWithNetworkComponent[i], NETWORK));
				if (networkComponent->GetClientId() == clientId)
				{
					CameraComponent* cameraComponent = reinterpret_cast<CameraComponent*>(manager->GetComponentOfEntity(entitiesWithNetworkComponent[i], CAMERA));
					game->GetRenderer().attachCamera(cameraComponent->GetCamera());
					break;
				}
			}
			game->GetNetwork().SetStatus(Status::CLIENT_LOADED);
		}
		else
		{
			std::cout << "FAILED " << static_cast<int>(message->responseType) << std::endl;
		}
	}

	// Clean up client memory using yojimbo
	void GameClient::CleanUp()
	{
		// Clean up client
		YOJIMBO_DELETE(yojimbo::GetDefaultAllocator(), GameAdapter, adapter);
		Status status = game->GetNetwork().GetStatus();
		if (status == Status::CLIENT_CONNECTED ||
			status == Status::CLIENT_LOADING ||
			status == Status::CLIENT_LOADED
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
			std::cout << "status: " << static_cast<int>(status) << std::endl;
			if (status == Status::CLIENT_LOADED || status == Status::CLIENT_LOADING)
				return;
			RequestMessage* message = static_cast<RequestMessage*>(client->CreateMessage(REQUEST_MESSAGE));
			message->requestType = RequestType::ENTITY_DATA;
			client->SendClientMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, message);
			std::cout << "MESSAGE TO SERVER WITH: TYPE = " << message->GetType() << ", MESSAGEID = ";
			std::cout << message->GetId() << ", REQUESTTYPE = " << static_cast<int>(message->requestType) << std::endl;
			game->GetNetwork().SetStatus(Status::CLIENT_LOADING);
			//client->ReleaseMessage(message);
		}
		else if (client->IsDisconnected())
		{
			if (status != Status::CLIENT_DISCONNECTED)
				game->GetNetwork().SetStatus(Status::CLIENT_DISCONNECTED);
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
		if (client->IsConnected())
		{
			info["Client Connected"] = "true";
			info["Server Address"] = std::to_string(*serverAddress.GetAddress4());
		}
		else
		{
			info["Client Connected"] = "false";
		}
		info["Client Index"] = std::to_string(client->GetClientIndex());
		info["Client Id"] = std::to_string(client->GetClientId());
		info["Is Loopback"] = std::to_string(client->IsLoopback());

		return info;
	}
}

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
		Start();
	}

	void GameServer::Start()
	{
		// Start server
		server->Start(maxClients);
		serverTime = yojimbo_time();
	}

	void GameServer::Update()
	{
		double time = yojimbo_time();
		if (serverTime + dt <= yojimbo_time()) {
			serverTime += dt;
		}
		else {
			//yojimbo_sleep(m_time - currentTime);
			return;
		}

		server->SendPackets();
		// update server and process messages
		server->ReceivePackets();
		ProcessMessages();
		server->AdvanceTime(dt);

		// ... process client inputs ...
		// ... update game ...
		// ... send game state to clients ...

	}

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

	void GameServer::ProcessMessage(int clientIndex, yojimbo::Message* message)
	{
		std::cout << "MESSAGE FROM CLIENT " << clientIndex << " WITH: TYPE = " << message->GetType() << ", MESSAGEID = " << message->GetId() << std::endl;
		if (message->GetType() == REQUEST_MESSAGE)
		{
			RequestMessage* derived = (RequestMessage*)message;
			HandleRequestMessage(clientIndex, derived->requestType);
		}
	}

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

	void GameServer::CleanUp()
	{
		// Clean up server
		YOJIMBO_DELETE(yojimbo::GetDefaultAllocator(), GameAdapter, adapter);
		server->Stop();
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
		uint16_t port = server->GetAddress().GetPort();
		info["Port"] = std::to_string(port);

		return info;
	}
}

#pragma once

#include <iostream>
#include <yojimbo.h>

#include "GameClient.hpp"

namespace Engine
{
	GameClient::GameClient(
		yojimbo::ClientServerConfig* config,
		GameAdapter* adapter,
		yojimbo::Address serverAddress)
		:
		config(config),
		adapter(adapter),
		serverAddress(serverAddress)
	{
		// Initialise  client
		client = YOJIMBO_NEW(yojimbo::GetDefaultAllocator(), yojimbo::Client,
			yojimbo::GetDefaultAllocator(),
			yojimbo::Address("0.0.0.0"),
			*config,
			*adapter,
			time);

		Connect();
	}

	void GameClient::Connect()
	{
		uint64_t clientId;
		yojimbo_random_bytes((uint8_t*)&clientId, 8);
		client->InsecureConnect(DEFAULT_PRIVATE_KEY, clientId, serverAddress);
	}

	void GameClient::Update()
	{
		//// check if client is disconnected
		//if (client->GetClientId() == 0)
		//{
		//	CleanUp();
		//	return;
		//}

		// update client
		double currentTime = yojimbo_time();
		if (client->GetTime() >= currentTime)
		{
			return;
		}

		client->AdvanceTime(client->GetTime() + dt);
		client->ReceivePackets();
		UpdateStatus();

		if (client->IsConnected())
		{
			ProcessMessages();

			// ... do connected stuff ...

			// send a message when space is pressed
			/*if (KeyIsDown(Key::SPACE)) {
				TestMessage* message = (TestMessage*)m_client.CreateMessage((int)GameMessageType::TEST);
				message->m_data = 42;
				m_client.SendMessage((int)GameChannel::RELIABLE, message);
			}*/
			//yojimbo::Message* message = adapter->factory->CreateMessage(1);
			//client->SendMessage(0, message);
		}

		client->SendPackets();
	}

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

	void GameClient::ProcessMessage(yojimbo::Message* message)
	{
		if (message->GetType() == REQUEST_MESSAGE)
		{
			HandleResponseMessage((RequestResponseMessage*)message);
		}
		std::cout << "Processing message from server with messageID " << message->GetId() << std::endl;
	}

	void GameClient::HandleResponseMessage(RequestResponseMessage* message)
	{
		int size = message->GetBlockSize();
		std::vector<uint8_t> data(size);
		message->GetBlockData();
		int sizes = message->GetBlockSize();
		/*if (message->responseType == ResponseType::ENTITY_DATA_RESPONSE)
		{
			uint8_t data
			message->GetBlockData()
			server->AttachBlockToMessage(clientIndex, message, block, bytes);
			server->SendServerMessage(clientIndex, yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, message);
			server->FreeBlock(clientIndex, block);*/
		//adapter->factory->ReleaseMessage(message);
		//server->ReleaseMessage(clientIndex, message);
	}

	void GameClient::CleanUp()
	{
		// Clean up client
		YOJIMBO_DELETE(yojimbo::GetDefaultAllocator(), GameAdapter, adapter);
		YOJIMBO_DELETE(yojimbo::GetDefaultAllocator(), Client, client);
		ShutdownYojimbo();
	}

	void GameClient::UpdateStatus()
	{
		if (client->IsConnecting() && status != Status::CLIENT_CONNECTING)
			status = Status::CLIENT_CONNECTING;
		else if (client->IsConnected() && status != Status::CLIENT_CONNECTED)
		{
			status = Status::CLIENT_CONNECTED;
			RequestMessage *message = (RequestMessage*)adapter->factory->CreateMessage(REQUEST_MESSAGE);
			message->requestType = RequestType::ENTITY_DATA;
			client->SendClientMessage(yojimbo::CHANNEL_TYPE_RELIABLE_ORDERED, message);
		}
		else if (client->IsDisconnected() && status != Status::CLIENT_DISCONNECTED)
			status = Status::CLIENT_DISCONNECTED;
		else if (client->ConnectionFailed() && status != Status::CLIENT_CONNECTION_FAILED)
			status = Status::CLIENT_CONNECTION_FAILED;
	}

	std::map<std::string, std::string> GameClient::GetInfo()
	{
		std::map<std::string, std::string> info;

		// Client Info
		info["Time"] = std::to_string(time);
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

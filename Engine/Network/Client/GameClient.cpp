#include <iostream>

#include "GameClient.hpp"
#include "../../../Game/GameLoop.hpp"

static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

GameClient::GameClient(
	yojimbo::ClientServerConfig* config,
	GameAdapter* adapter,
	yojimbo::Address serverAddress)
	:
	config(config),
	adapter(adapter)
{
	// Initialise  client
	client = YOJIMBO_NEW(yojimbo::GetDefaultAllocator(), yojimbo::Client,
		yojimbo::GetDefaultAllocator(),
		yojimbo::Address("0.0.0.0"),
		*config,
		*adapter,
		time);

	Connect(serverAddress);
}

void GameClient::Connect(yojimbo::Address serverAddress)
{
	uint64_t clientId;
	yojimbo_random_bytes((uint8_t*)&clientId, 8);
	client->InsecureConnect(DEFAULT_PRIVATE_KEY, clientId, serverAddress);
	Run();
}

void GameClient::Run()
{
	//initialiseGame();
	//runGameLoop(this);

	//float fixedDt = 1.0f / 120.0f;
	//while (true)
	//{
	//	client->SendPackets();

	//	client->ReceivePackets();

	//	if (client->IsDisconnected())
	//		break;

	//	time += fixedDt;

	//	client->AdvanceTime(time);

	//	if (client->ConnectionFailed())
	//		break;

	//	/*if(client->IsConnecting())
	//		std::cout << "Connecting to server...\n";*/

	//	yojimbo_sleep(fixedDt);
	//	GameMessage* message = (GameMessage*)adapter->factory->CreateMessage(GameMessageType::GAME_MESSAGE);
	//	message->sequence = 42;
	//	client->SendMessage(1, message);
	//	//adapter->factory->ReleaseMessage(message);
	//}
	//Update(fixedDt);
	/*while (client->IsConnected())
	{
		double currentTime = yojimbo_time();
		if (client->GetTime() <= currentTime)
		{
			Update(fixedDt);
		}
		else
		{
			yojimbo_sleep(client->GetTime() - currentTime);
		}
	}*/
}

int GameClient::Update()
{
	// check if client is disconnected
	if (client->GetClientId() == 0)
	{
		CleanUp();
		return 1;
	}

    // update client
	double currentTime = yojimbo_time();
	if (client->GetTime() >= currentTime)
	{
		return 0;
	}

    client->AdvanceTime(client->GetTime() + dt);
    client->ReceivePackets();

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
        yojimbo::Message* message = adapter->factory->CreateMessage(1);
		client->SendMessage(0, message);
    }

    client->SendPackets();
	return 0;
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
    std::cout << "Processing message from server with messageID " << message->GetId() << std::endl;
}

void GameClient::CleanUp()
{
	// Clean up client
	YOJIMBO_DELETE(yojimbo::GetDefaultAllocator(), GameAdapter, adapter);
	YOJIMBO_DELETE(yojimbo::GetDefaultAllocator(), Client, client);
	ShutdownYojimbo();
}
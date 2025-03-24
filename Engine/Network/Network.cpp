#pragma once

#include "Network.hpp"

namespace Engine
{
    Network::Network()
    {
        if (!InitializeYojimbo())
        {
            std::cerr << "Failed to initialize Yojimbo\n";
            adapter = YOJIMBO_NEW(yojimbo::GetDefaultAllocator(), GameAdapter);
        }
        else
        {
            std::cout << "Yojimbo initialized\n";
        }
    }

    void Network::InitializeClient(yojimbo::Address serverAddress)
    {
        if (!initialized)
        {
            networkType = std::make_unique<GameClient>(&config, adapter, serverAddress);
            initialized = true;
        }
        else
        {
            std::cout << "Network already initialised as " << typeid(*networkType).name() << std::endl;
        }
    }

    void Network::InitializeServer(Game* game, uint16_t port, int maxClients)
    {
        if (!initialized)
        {
            networkType = std::make_unique<GameServer>(&config, adapter, GetLocalIPAddress(port), maxClients, game);
            initialized = true;
        }
        else
        {
            std::cout << "Network already initialised as " << typeid(*networkType).name() << std::endl;
        }
    }

    void Network::Reset()
    {
        networkType->CleanUp();
        networkType.reset();
        initialized = false;
    }
}
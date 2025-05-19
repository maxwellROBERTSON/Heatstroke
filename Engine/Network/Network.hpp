#pragma once

#include <vector>
#include <string>

#include "Helpers/GameConfig.hpp"
#include "Helpers/GetLocalIPAddress.hpp"
#include "../Core/Game.hpp"

namespace Engine
{
    class Game;
}

#include "Client/GameClient.hpp"
#include "Server/GameServer.hpp"

namespace Engine
{
	class Network
	{
    public:
        // Constructor
        Network() {};

        // Getters

        // Get debugging info for the network
        std::map<std::string, std::string> GetNetworkInfo();

        // Get status of the network
        Status& GetStatus() { return status; }

        // Get status string of the network
        std::string GetStatusString();

        GameNetworkType* GetNetworkTypePointer() { return networkType.get(); }

        // Setters

        // Initialize yojimbo, debug and adapter
        void InitializeNetwork();

        // InitializeNetwork() and create client object
        void InitializeClient(Game*, yojimbo::Address);

        // InitializeNetwork() and create server object
        void InitializeServer(Game*, uint16_t, int);

        // Update network if some form of connection
        void Update(float);

        // Reset network if not uninitialized, clean up and reset pointer
        void Reset();

        // Set status of the network
        void SetStatus(Status s) { status = s; }

    private:
        Status status = Status::NETWORK_UNINITIALIZED;
        std::unique_ptr<GameNetworkType> networkType;  // Holds either GameClient or GameServer
        GameConfig config;
        GameAdapter* adapter;
	};
}
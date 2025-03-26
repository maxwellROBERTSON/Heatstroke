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
        Network() {};
        void InitializeNetwork();
        void InitializeClient(yojimbo::Address);
        void InitializeServer(Game*, uint16_t, int);
        void Update();
        void Reset();

        // Getters
        std::map<std::string, std::string> GetNetworkInfo();
        inline bool isInitialized() { return initialized; }
        inline Status& GetStatus() { return networkType->GetStatus(); }
        inline std::string GetStatusString() { return networkType->GetStatusString(); }

    private:
        std::unique_ptr<GameNetworkType> networkType;  // Holds either GameClient or GameServer
        bool initialized = false;
        yojimbo::ClientServerConfig config;
        GameAdapter* adapter;
	};
}
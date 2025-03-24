#pragma once

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
        Network();
        void InitializeClient(yojimbo::Address);
        void InitializeServer(Game*, uint16_t, int);
        void Reset();

    private:
        std::unique_ptr<GameNetworkType> networkType;  // Holds either GameClient or GameServer
        bool initialized = false;
        yojimbo::ClientServerConfig config;
        GameAdapter* adapter;
	};
}
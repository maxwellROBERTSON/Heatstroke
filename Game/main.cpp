//#include <cstdio>
//#include <memory>
//#include <chrono>
//#include <string>

#include <yojimbo.h>

#include "../Engine/Network/Helpers/GameAdapter.hpp"
#include "../Engine/Network/Helpers/GameConfig.hpp"
#include "../Engine/Network/Helpers/GetLocalIPAddress.hpp"
#include "../Engine/Network/Server/GameServer.hpp"
#include "../Engine/Network/Client/GameClient.hpp"

int main()
{
    if (!InitializeYojimbo())
    {
        std::cerr << "Failed to initialize Yojimbo\n";
        exit(1);
    }
    else
    {
        std::cout << "Yojimbo initialized\n";
    }

#ifdef DEBUG
    yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
#else
    yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
#endif

    // Make adapter
    GameAdapter* adapter = YOJIMBO_NEW(yojimbo::GetDefaultAllocator(), GameAdapter);

    // Make config
    yojimbo::ClientServerConfig config;

#ifdef CLIENT
    // Make and start client
    GameClient client(&config, adapter, GetLocalIPAddress(3000));
#else
    // Make and start server
    GameServer server(&config, adapter, GetLocalIPAddress(3000), 2);
#endif
}

#include "Network.hpp"

namespace Engine
{
    void Network::InitializeNetwork()
    {
        if (!InitializeYojimbo())
        {
            std::cerr << "Failed to initialize Yojimbo.\n";
        }
        else
        {
            std::cout << "Yojimbo initialized.\n";
            adapter = YOJIMBO_NEW(yojimbo::GetDefaultAllocator(), GameAdapter);
            std::cout << "Adapter created.\n";
        }
#ifdef _DEBUG
        yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
#else
        yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
#endif
    }

    void Network::InitializeClient(Game* game, yojimbo::Address serverAddress)
    {
        if (!initialized)
        {
            InitializeNetwork();
            networkType = std::make_unique<GameClient>(&config, adapter, serverAddress, game);
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
            InitializeNetwork();
            networkType = std::make_unique<GameServer>(&config, adapter, GetLocalIPAddress(port), maxClients, game);
            initialized = true;
        }
        else
        {
            std::cout << "Network already initialised as " << typeid(*networkType).name() << std::endl;
        }
    }

    void Network::Update()
    {
        if (initialized)
            networkType->Update();
    }

    void Network::Reset()
    {
        if (initialized)
        {
            networkType->CleanUp();
            networkType.reset();
            initialized = false;
            std::cout << "Network reset." << std::endl;
        }
    }

    std::map<std::string, std::string> Network::GetNetworkInfo()
    {
        std::map<std::string, std::string> info = networkType->GetInfo();

        // Config Info
        info["ProtocolId"] = std::to_string(config.protocolId);
        info["Timeout"] = std::to_string(config.timeout);
        info["ClientMemory"] = std::to_string(config.clientMemory);
        info["ServerGlobalMemory"] = std::to_string(config.serverGlobalMemory);
        info["ServerPerClientMemory"] = std::to_string(config.serverPerClientMemory);
        info["NetworkSimulator"] = config.networkSimulator ? "true" : "false";
        info["MaxSimulatorPackets"] = std::to_string(config.maxSimulatorPackets);
        info["FragmentPacketsAbove"] = std::to_string(config.fragmentPacketsAbove);
        info["PacketFragmentSize"] = std::to_string(config.packetFragmentSize);
        info["MaxPacketFragments"] = std::to_string(config.maxPacketFragments);
        info["PacketReassemblyBufferSize"] = std::to_string(config.packetReassemblyBufferSize);
        info["AckedPacketsBufferSize"] = std::to_string(config.ackedPacketsBufferSize);
        info["ReceivedPacketsBufferSize"] = std::to_string(config.receivedPacketsBufferSize);
        info["RttSmoothingFactor"] = std::to_string(config.rttSmoothingFactor);

        return info;
    }
}
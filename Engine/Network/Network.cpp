#include "Network.hpp"

namespace Engine
{
    // Getters

    // Get debugging info for the network
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

    // Get status string of the network
    std::string Network::GetStatusString()
    {
        switch (status)
        {
        case Status::NETWORK_UNINITIALIZED:
            return "Network Uninitialised.";
        case Status::CLIENT_CONNECTING:
            return "Connecting.";
        case Status::CLIENT_CONNECTED:
            return "Connected.";
        case Status::CLIENT_DISCONNECTED:
            return "Disconnected.";
        case Status::CLIENT_CONNECTION_FAILED:
            return "Connection Failed.";
        case Status::CLIENT_LOADING:
            return "Loading Scene.";
        case Status::CLIENT_LOADED:
            return "Loaded Scene.";
        default:
            return "Unknown";
        }
    }

    // Initialize yojimbo, debug and adapter
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
        yojimbo_log_level(YOJIMBO_LOG_LEVEL_DEBUG);
#else
        yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
#endif
        status = Status::NETWORK_INITIALIZED;
    }

    // InitializeNetwork() and create client object
    void Network::InitializeClient(Game* game, yojimbo::Address serverAddress)
    {
        if (status == Status::NETWORK_UNINITIALIZED)
        {
            InitializeNetwork();
            networkType = std::make_unique<GameClient>(&config, adapter, serverAddress, game);
        }
        else
        {
            std::cout << "Network already initialised as " << typeid(*networkType).name() << std::endl;
        }
    }

    // InitializeNetwork() and create server object
    void Network::InitializeServer(Game* game, uint16_t port, int maxClients)
    {
        if (status == Status::NETWORK_UNINITIALIZED)
        {
            InitializeNetwork();
            networkType = std::make_unique<GameServer>(&config, adapter, GetLocalIPAddress(port), maxClients, game);
        }
        else
        {
            std::cout << "Network already initialised as " << typeid(*networkType).name() << std::endl;
        }
    }

    // Update network if some form of connection
    void Network::Update()
    {
        if (status != Status::NETWORK_UNINITIALIZED &&
            status != Status::NETWORK_INITIALIZED &&
            status != Status::CLIENT_DISCONNECTED &&
            status != Status::CLIENT_CONNECTION_FAILED
            )
        {
            networkType->Update();
        }
    }

    // Reset network if not uninitialized, clean up and reset pointer
    void Network::Reset()
    {
        if (status == Status::NETWORK_UNINITIALIZED)
        {
            std::cout << "Network uninitialized." << std::endl;
            return;
        }
        else if (status == Status::NETWORK_INITIALIZED)
        {
            networkType.reset();
            std::cout << "Network cleaned and reset." << std::endl;
        }
        else
        {
            networkType->CleanUp();
            networkType.reset();
            std::cout << "Network reset." << std::endl;
        }
        status = Status::NETWORK_UNINITIALIZED;
    }
}
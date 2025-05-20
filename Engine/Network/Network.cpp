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
        case Status::CLIENT_LOADING_DATA:
            return "Loading Scene.";
        case Status::CLIENT_INITIALIZING_DATA:
            return "Initializing Scene.";
        case Status::CLIENT_ACTIVE:
            return "Client ready to communicate with server.";
        default:
            return "Unknown";
        }
    }

    // Initialize yojimbo, debug and adapter
    void Network::InitializeNetwork()
    {
        if (!InitializeYojimbo())
        {
            DLOG("Failed to initialize Yojimbo.");
            return;
        }
        else
        {
            DLOG("Yojimbo initialized.");
            adapter = YOJIMBO_NEW(yojimbo::GetDefaultAllocator(), GameAdapter);
            DLOG("Adapter created.");
        }
#ifdef _DEBUG
        yojimbo_log_level(YOJIMBO_LOG_LEVEL_INFO);
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
            if (true)
            {
                config;
                int i = 0;
            }
        }
        else
        {
            DLOG("Network already initialised as " << typeid(*networkType).name());
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
            DLOG("Network already initialised as " << typeid(*networkType).name());
        }
    }

    // Update network if some form of connection
    void Network::Update(float timeDelta)
    {
        if (status != Status::NETWORK_UNINITIALIZED &&
            status != Status::NETWORK_INITIALIZED &&
            status != Status::CLIENT_DISCONNECTED &&
            status != Status::CLIENT_CONNECTION_FAILED
            )
        {
            networkType->Update(timeDelta);
        }
    }

    // Reset network if not uninitialized, clean up and reset pointer
    void Network::Reset()
    {
        if (status == Status::NETWORK_UNINITIALIZED)
        {
            DLOG("Network uninitialized.");
            return;
        }
        else if (status == Status::NETWORK_INITIALIZED)
        {
            networkType.reset();
            DLOG("Network cleaned and reset.");
        }
        else
        {
            networkType->CleanUp();
            networkType.reset();
            DLOG("Network reset.");
        }
        ShutdownYojimbo();
        status = Status::NETWORK_UNINITIALIZED;
    }
}
#pragma once

#include <map>

#include "GameAdapter.hpp"

#include "../ECS/Components/Component.hpp"
#include "../ECS/Components/AudioComponent.hpp"
#include "../ECS/Components/CameraComponent.hpp"
#include "../ECS/Components/ChildrenComponent.hpp"
#include "../ECS/Components/NetworkComponent.hpp"
#include "../ECS/Components/PhysicsComponent.hpp"
#include "../ECS/Components/RenderComponent.hpp"

namespace Engine
{
	// Status of the network
    enum class Status : int
    {
        NETWORK_UNINITIALIZED,
		NETWORK_INITIALIZED,
		CLIENT_INITIALIZED,
		SERVER_INITIALIZED,
        CLIENT_CONNECTING,
        CLIENT_CONNECTED,
        CLIENT_DISCONNECTED,
        CLIENT_CONNECTION_FAILED,
        CLIENT_LOADING_DATA,
		CLIENT_INITIALIZING_DATA,
        CLIENT_ACTIVE
    };

	static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

	struct GameConfig : public yojimbo::ClientServerConfig
	{
		GameConfig()
		{
			channel[1].type = yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED;
			channel[1].disableBlocks = false;
			channel[1].maxBlockSize = 1024;
			channel[1].packetBudget = 1200;
			channel[1].messageSendQueueSize = 64;
			channel[1].messageReceiveQueueSize = 64;
			channel[1].maxMessagesPerPacket = 16;
		}
	};

	// Parent class of client and server
	class GameNetworkType
	{
	public:
		virtual ~GameNetworkType() = default;

		virtual void Update() = 0;
		virtual void CleanUp() = 0;
		virtual std::map<std::string, std::string> GetInfo() = 0;
		virtual void UpdateStatus() = 0;
		virtual void ReadyToSendResetMessage() = 0;
	protected:
		float dt = 1.0f / 120.0f;
	};
}
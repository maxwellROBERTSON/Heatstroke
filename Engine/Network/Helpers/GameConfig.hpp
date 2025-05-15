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
			channel[1].maxBlockSize = maxPacketSize;
			channel[1].blockFragmentSize = maxPacketSize;
			/*channel[yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED].disableBlocks = false;
			channel[yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED].sentPacketBufferSize = 1024;
			channel[yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED].messageSendQueueSize = 1024;
			channel[yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED].messageReceiveQueueSize = 1024;
			channel[yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED].maxMessagesPerPacket = 256;
			channel[yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED].packetBudget = -1;
			channel[yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED].messageResendTime = 0.1f;
			channel[yojimbo::CHANNEL_TYPE_UNRELIABLE_UNORDERED].blockFragmentResendTime = 0.25f;*/
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
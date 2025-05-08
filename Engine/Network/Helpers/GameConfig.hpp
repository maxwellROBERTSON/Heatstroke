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

	// Parent class of client and server
	class GameNetworkType
	{
	public:
		virtual ~GameNetworkType() = default;

		virtual void Update() = 0;
		virtual void CleanUp() = 0;
		virtual std::map<std::string, std::string> GetInfo() = 0;
		virtual void UpdateStatus() = 0;
	protected:
		float dt = 1.0f / 120.0f;
	};
}
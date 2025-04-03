#pragma once

#include "GameAdapter.hpp"

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
        CLIENT_LOADING,
        CLIENT_LOADED
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
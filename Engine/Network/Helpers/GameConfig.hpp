#pragma once

#include "GameAdapter.hpp"

namespace Engine
{
    enum class Status : int
    {
        CLIENT_CONNECTING = 0,
        CLIENT_CONNECTED = 1,
        CLIENT_DISCONNECTED = 2,
        CLIENT_CONNECTION_FAILED = 3,
        CLIENT_LOADED = 4
    };

	static const uint8_t DEFAULT_PRIVATE_KEY[yojimbo::KeyBytes] = { 0 };

	class GameNetworkType
	{
	public:
		virtual ~GameNetworkType() = default;

		virtual void Update() = 0;
		virtual void CleanUp() = 0;
		virtual std::map<std::string, std::string> GetInfo() = 0;
		virtual void UpdateStatus() = 0;
        inline Status& GetStatus() { return status; }
        inline std::string GetStatusString()
        {
            switch (status)
            {
            case Status::CLIENT_CONNECTING:
                return "Connecting.";
            case Status::CLIENT_CONNECTED:
                return "Connected.";
            case Status::CLIENT_DISCONNECTED:
                return "Disconnected.";
            case Status::CLIENT_CONNECTION_FAILED:
                return "Connection Failed.";
            case Status::CLIENT_LOADED:
                return "Loaded Scene.";
            default:
                return "Unknown";
            }
        }
        virtual void SetStatus(Status s) { status = s; }

        RequestType request;
        Status status;
	};
}
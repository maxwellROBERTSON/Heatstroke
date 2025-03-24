#pragma once

#include "GameAdapter.hpp"

namespace Engine
{
    enum class Status : int
    {
        ClientConnecting = 0,
        ClientConnected = 1,
        ClientDisconnected = 2,
        ClientConnectionFailed = 3,
        ClientLoaded = 4
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
            case Status::ClientConnecting:
                return "Connecting.";
            case Status::ClientConnected:
                return "Connected.";
            case Status::ClientDisconnected:
                return "Disconnected.";
            case Status::ClientConnectionFailed:
                return "Connection Failed.";
            case Status::ClientLoaded:
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
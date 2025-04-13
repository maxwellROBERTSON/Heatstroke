#pragma once

//#define NOMINMAX           // Avoid conflicts with Windows macros like min/max
//#define _WIN32_WINNT 0x0601 // Optional, depending on your Windows version
//#define WINUSERAPI

#include <yojimbo.h>
#include <iostream>

namespace Engine
{
    // Retrieves the number of bits required for a message based on a sequence number
    inline int GetNumBitsForMessage(uint16_t sequence)
    {
        static int messageBitsArray[] = { 1, 320, 120, 4, 256, 45, 11, 13, 101, 100, 84, 95, 203, 2, 3, 8, 512, 5, 3, 7, 50 };
        const int modulus = sizeof(messageBitsArray) / sizeof(int);
        const int index = sequence % modulus;
        return messageBitsArray[index];
    }

    /*MESSAGE_RECEIVED,
    REQUEST_ENTITY_DATA,
    RESPONSE_ENTITY_DATA,
    REQUEST_PLAYER_STATS,
    RESPONSE_PLAYER_STATS,
    REQUEST_GAME_STATE,
    RESPONSE_GAME_STATE,
    REQUEST_CHAT_MESSAGES,
    RESPONSE_CHAT_MESSAGES,
    COUNT*/

    class MessageReceived : public yojimbo::Message
    {
    public:
        MessageReceived() {}

        template <typename Stream> bool Serialize(Stream& stream)
        {
            return true;
        }

        YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
    };

    class RequestEntityData : public yojimbo::Message
    {
    public:
        RequestEntityData() {}

        template <typename Stream> bool Serialize(Stream& stream)
        {
            return true;
        }

        YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
    };

    class ResponseEntityData : public yojimbo::BlockMessage
    {
    public:
        ResponseEntityData() {}

        template <typename Stream> bool Serialize(Stream& stream)
        {
            int size = GetBlockSize();
            uint8_t* block = GetBlockData();
            for (int i = 0; i < size; i++)
            {
                serialize_bits(stream, block[i], 8);
            }

            return true;
        }

        YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
    };

    class ClientUpdateEntityData : public yojimbo::BlockMessage
    {
    public:
        ClientUpdateEntityData() {}

        template <typename Stream> bool Serialize(Stream& stream)
        {
            int size = GetBlockSize();
            uint8_t* block = GetBlockData();
            for (int i = 0; i < size; i++)
            {
                serialize_bits(stream, block[i], 8);
            }

            return true;
        }

        YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS();
    };

    enum GameMessageType
    {
        MESSAGE_RECEIVED,
        REQUEST_ENTITY_DATA,
        RESPONSE_ENTITY_DATA,
        CLIENT_UPDATE_ENTITY_DATA,
        NUM_GAME_MESSAGE_TYPES
    };

    const std::string GameMessageTypeStrings[] =
    {
        "MESSAGE_RECEIVED",
        "REQUEST_ENTITY_DATA",
        "RESPONSE_ENTITY_DATA",
        "CLIENT_UPDATE_ENTITY_DATA",
        "NUM_GAME_MESSAGE_TYPES"
    };

    // Factory class responsible for creating game messages
    class GameMessageFactory : public yojimbo::MessageFactory
    {
    public:
        GameMessageFactory(yojimbo::Allocator& allocator) : MessageFactory(allocator, NUM_GAME_MESSAGE_TYPES) {}
        yojimbo::Message* CreateMessageInternal(int type)
        {
            yojimbo::Message* message;
            yojimbo::Allocator& allocator = GetAllocator();
            (void)allocator;
            switch (type)
            {
            case MESSAGE_RECEIVED:
                message = YOJIMBO_NEW(allocator, MessageReceived);
                if (!message)
                    return nullptr;
                SetMessageType(message, type);
                return message;
            case REQUEST_ENTITY_DATA:
                message = YOJIMBO_NEW(allocator, RequestEntityData);
                if (!message)
                    return nullptr;
                SetMessageType(message, type);
                return message;
            case RESPONSE_ENTITY_DATA:
                message = YOJIMBO_NEW(allocator, ResponseEntityData);
                if (!message)
                    return nullptr;
                SetMessageType(message, type);
                return message;
            case CLIENT_UPDATE_ENTITY_DATA:
                message = YOJIMBO_NEW(allocator, ClientUpdateEntityData);
                if (!message)
                    return nullptr;
                SetMessageType(message, type);
                return message;
            default:
                return nullptr;
            }
        }
    };

    // Adapter class handling server-client interactions and message factories
    class GameAdapter : public yojimbo::Adapter
    {
    public:
        GameAdapter() : server(nullptr), factory(nullptr) {}

        void SetServer(yojimbo::Server* server) {
            this->server = server;
        }
        GameMessageFactory* CreateMessageFactory(yojimbo::Allocator& allocator) override
        {
            factory = YOJIMBO_NEW(allocator, GameMessageFactory, allocator);
            return factory;
        }
        void DestroyMessageFactory(yojimbo::Allocator& allocator)
        {
            YOJIMBO_DELETE(allocator, GameMessageFactory, factory);
        }
        void OnServerClientConnected(int clientIndex) override {
            std::cout << "Client connected: " << clientIndex << std::endl;
        }
        void OnServerClientDisconnected(int clientIndex) override {
            std::cout << "Client disconnected: " << clientIndex << std::endl;
        }
        GameMessageFactory* factory;

    private:
        yojimbo::Server* server;
    };
}

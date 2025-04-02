#pragma once

//#define NOMINMAX           // Avoid conflicts with Windows macros like min/max
//#define _WIN32_WINNT 0x0601 // Optional, depending on your Windows version
//#define WINUSERAPI

#include <yojimbo.h>
#include <iostream>

namespace Engine
{
    inline int GetNumBitsForMessage(uint16_t sequence)
    {
        static int messageBitsArray[] = { 1, 320, 120, 4, 256, 45, 11, 13, 101, 100, 84, 95, 203, 2, 3, 8, 512, 5, 3, 7, 50 };
        const int modulus = sizeof(messageBitsArray) / sizeof(int);
        const int index = sequence % modulus;
        return messageBitsArray[index];
    }

    class GameMessage : public yojimbo::Message
    {
    public:
        GameMessage(uint16_t sequence = 0) : sequence(sequence) {}
        uint16_t sequence;

        template <typename Stream> bool Serialize(Stream& stream)
        {
            serialize_bits(stream, sequence, 16);

            int numBits = GetNumBitsForMessage(sequence);
            int numWords = numBits / 32;
            uint32_t dummy = 0;
            for (int i = 0; i < numWords; ++i)
                serialize_bits(stream, dummy, 32);
            int numRemainderBits = numBits - numWords * 32;
            if (numRemainderBits > 0)
                serialize_bits(stream, dummy, numRemainderBits);

            return true;
        }

        YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
    };

    enum class RequestType : uint8_t
    {
        ENTITY_DATA,
        PLAYER_STATS,
        GAME_STATE,
        CHAT_MESSAGES,
        COUNT
        // Add more request types as needed
    };

    enum class ResponseType : uint8_t
    {
        ENTITY_DATA_RESPONSE,
        PLAYER_STATS_RESPONSE,
        GAME_STATE_RESPONSE,
        CHAT_MESSAGES_RESPONSE,
        COUNT
        // Add more response types as needed
    };

    class RequestMessage : public yojimbo::Message
    {
    public:
        RequestMessage() : requestType(RequestType::COUNT) {}

        template <typename Stream> bool Serialize(Stream& stream)
        {
            uint8_t requestTypeValue = static_cast<uint8_t>(requestType);

            serialize_bits(stream, requestTypeValue, 8);

            if (Stream::IsReading)
            {
                requestType = static_cast<RequestType>(requestTypeValue);
            }

            return true;
        }

        YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()

            RequestType requestType;
    };

    class GameBlockMessage : public yojimbo::BlockMessage
    {
    public:
        GameBlockMessage(uint16_t sequence = 0) : sequence(sequence) {}
        uint16_t sequence;

        template <typename Stream> bool Serialize(Stream& stream)
        {
            serialize_bits(stream, sequence, 16);
            return true;
        }

        YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()
    };

    // For each entity:
    // Component vector of int = registry.GetNumComponents * sizeof(int)
    // Matrix = 16 * sizeof(float)
    // If rendercomp:
    //  Model index = sizeof(int0
    // If camera:
    // fov + near + far + vec3(pos) + vec3(front_dir) = 9 * float
    // If network:
    // clientid = int

    class RequestResponseMessage : public yojimbo::BlockMessage
    {
    public:
        RequestResponseMessage() : responseType(ResponseType::COUNT){}
        
        template <typename Stream> bool Serialize(Stream& stream)
        {
            uint8_t responseTypeValue = static_cast<uint8_t>(responseType);

            serialize_bits(stream, responseTypeValue, 8);

            if (Stream::IsReading)
            {
                responseType = static_cast<ResponseType>(responseTypeValue);
            }

            int size = GetBlockSize();
            uint8_t* block = GetBlockData();
            for (int i = 0; i < size; i++)
            {
                serialize_bits(stream, block[i], 8);
            }

            return true;
        }

        YOJIMBO_VIRTUAL_SERIALIZE_FUNCTIONS()

        ResponseType responseType;
    };

    enum GameMessageType
    {
        GAME_MESSAGE,
        REQUEST_MESSAGE,
        GAME_BLOCK_MESSAGE,
        REQUEST_RESPONSE_MESSAGE,
        NUM_GAME_MESSAGE_TYPES
    };

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
            case GAME_MESSAGE:
                message = YOJIMBO_NEW(allocator, GameMessage);
                if (!message)
                    return nullptr;
                SetMessageType(message, type);
                return message;
            case REQUEST_MESSAGE:
                message = YOJIMBO_NEW(allocator, RequestMessage);
                if (!message)
                    return nullptr;
                SetMessageType(message, type);
                return message;
            case GAME_BLOCK_MESSAGE:
                message = YOJIMBO_NEW(allocator, GameBlockMessage);
                if (!message)
                    return nullptr;
                SetMessageType(message, type);
                return message;
            case REQUEST_RESPONSE_MESSAGE:
                message = YOJIMBO_NEW(allocator, RequestResponseMessage);
                if (!message)
                    return nullptr;
                SetMessageType(message, type);
                return message;
            default:
                return nullptr;
            }
        }
    };

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

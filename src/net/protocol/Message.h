#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <nlohmann/json.hpp>
#include "net/protocol/MessageType.h"

namespace net::protocol {
    constexpr size_t kTypeFieldSize = sizeof(MessageType); // 1 byte
    constexpr size_t kLengthFieldSize = sizeof(uint32_t);  // 4 bytes
    constexpr size_t kHeaderSize = kTypeFieldSize + kLengthFieldSize; // 5 bytes
    
    using json = nlohmann::json;

    class Message {
    public:
        MessageType type;
        json j;

        Message();
        Message(MessageType t, const json& body);

        // Serialization (to bytes)
        std::vector<uint8_t> encode() const;

        // Deserialization (from bytes)
        static Message decode(MessageType type, const std::vector<uint8_t>& payload);

        // Convenience wrappers
        static Message makeRequest(uint32_t id, const std::string& method, const json& params);
        static Message makeResponse(uint32_t id, const json& result);
        static Message makeError(uint32_t id, int code, const std::string& msg);
        static Message makePush(const json& pushBody);

        // Timestamp helper
        static uint64_t nowTimestamp();
    };

} // namespace net::protocol

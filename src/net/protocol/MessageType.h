#pragma once

#include <cstdint>

namespace net::protocol {

enum class MessageType : uint8_t {
    Request  = 0,   // Client → Server (contains: id, method, params)
    Response = 1,   // Server → Client (contains: id, result OR error)
    Push     = 2    // Server → Client (contains: id=0, push {...})
};

} // namespace net::protocol

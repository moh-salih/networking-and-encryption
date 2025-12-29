#include "net/protocol/Message.h"
#include <chrono>
#include <cstring>
#include <stdexcept>

namespace net::protocol {

Message::Message()
    : type(MessageType::Request), j(json::object()) {}

Message::Message(MessageType t, const json& body)
    : type(t), j(body) {}



// Timestamp (UNIX epoch, seconds)
uint64_t Message::nowTimestamp() {
    using namespace std::chrono;
    return duration_cast<seconds>(system_clock::now().time_since_epoch()).count();
}


// Encoding: [1 byte type][4 bytes length BE][JSON pretty body]
std::vector<uint8_t> Message::encode() const {
    // Pretty-print JSON
    std::string body = j.dump(4);

    uint32_t len = static_cast<uint32_t>(body.size());
    uint32_t lenBE =
        ((len & 0x000000FFu) << 24) |
        ((len & 0x0000FF00u) << 8)  |
        ((len & 0x00FF0000u) >> 8)  |
        ((len & 0xFF000000u) >> 24);

    std::vector<uint8_t> out;
    out.reserve(1 + 4 + len);

    // MessageType
    out.push_back(static_cast<uint8_t>(type));

    // Length (big-endian)
    uint8_t* p = reinterpret_cast<uint8_t*>(&lenBE);
    out.insert(out.end(), p, p + 4);

    // JSON payload
    out.insert(out.end(), body.begin(), body.end());

    return out;
}


// Decode a message from its JSON payload
// Type is already known from the header

Message Message::decode(MessageType type, const std::vector<uint8_t>& payload) {
    if (payload.empty()) {
        throw std::runtime_error("Empty JSON payload");
    }

    std::string body(payload.begin(), payload.end());
    json parsed = json::parse(body);

    return Message(type, parsed);
}


// Convenience Builders

Message Message::makeRequest(uint32_t id, const std::string& method, const json& params) {
    json j;
    j["id"]        = id;
    j["method"]    = method;
    j["params"]    = params;
    j["timestamp"] = nowTimestamp();
    return Message(MessageType::Request, j);
}

Message Message::makeResponse(uint32_t id, const json& result) {
    json j;
    j["id"]        = id;
    j["result"]    = result;
    j["timestamp"] = nowTimestamp();
    return Message(MessageType::Response, j);
}

Message Message::makeError(uint32_t id, int code, const std::string& msg) {
    json j;
    j["id"]        = id;
    j["error"]     = {
        {"code", code},
        {"message", msg}
    };
    j["timestamp"] = nowTimestamp();
    return Message(MessageType::Response, j);
}

Message Message::makePush(const json& pushBody) {
    json j;
    j["id"]        = 0; // pushes always use id = 0
    j["push"]      = pushBody;
    j["timestamp"] = nowTimestamp();
    return Message(MessageType::Push, j);
}

} // namespace net::protocol

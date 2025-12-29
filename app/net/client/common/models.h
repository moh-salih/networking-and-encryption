#pragma once
#include <string>
#include <stdint.h>

struct ChatMessage {
    uint32_t uid = 0;
    std::string fromName;
    std::string text;
    bool isPrivate = false;
};

struct User {
    uint32_t uid = 0;
    std::string name;
    int unreadCount = 0;
};

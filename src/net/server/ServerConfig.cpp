#include "net/server/ServerConfig.h"

namespace net::server{

    
    bool ServerConfig::isValid() const {
        if (port == 0) return false;
        if (mode == ServerMode::Secure)
        return !certFile.empty() && !keyFile.empty();
        return true;
    }
}
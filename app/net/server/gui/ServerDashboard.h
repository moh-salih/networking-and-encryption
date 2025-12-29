#pragma once

#include <memory>
#include <thread>
#include <deque>
#include <mutex>
#include <chrono>

#include <imgui.h>

#include "net/server/Router.h"
#include "net/server/SessionManager.h"
#include "net/protocol/Message.h"
#include "net/server/ServerController.h"
#include "net/server/ServerConfig.h"
#include "app/net/server/common/ServerAppContext.h"
#include "app/net/common/IApplication.h"


// --- Thread-Safe Logger Definition ---
struct LogEntry {
    std::string time;
    std::string msg;
    ImVec4 color;
};

class AppLogger {
    std::deque<LogEntry> mLogs;
    std::mutex mMutex;
    const size_t MAX_LOGS = 200;

public:
    void add(const std::string& msg, ImVec4 color = ImVec4(1,1,1,1));
    void draw();
};
// --- End Logger Definition ---

using ServerConfig = net::server::ServerConfig;
class ServerDashboard: public IApplication{
public:
    ServerDashboard(ServerConfig config);
    ~ServerDashboard();

    // IApplication interface
    void render() override;
    void update() override;
    void applyTheme() override;
    void initialize() override;

    // Server Control
    void start();
    void stop();

private:
    void renderStatsBar();
    void renderControls();
    void renderClientList();

private:

    app::server::ServerAppContext mCtx;


    // Server Config
    ServerConfig mConfig;


    // State & UI
    bool mIsRunning = false;
    AppLogger mLogger;
    std::chrono::steady_clock::time_point mStartTime;
    char mBroadcastBuf[256] = {};
};
#include "app/net/server/gui/ServerDashboard.h"
#include <iostream>
#include <iomanip>
#include <sstream>

using json = net::protocol::json;
using Message = net::protocol::Message;
namespace net_server = net::server;

// --- Logger Implementation ---

void AppLogger::add(const std::string& msg, ImVec4 color) {
    std::lock_guard<std::mutex> lock(mMutex);
    
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "[%H:%M:%S] ");

    mLogs.push_back({ss.str(), msg, color});
    if (mLogs.size() > MAX_LOGS) mLogs.pop_front();
}

void AppLogger::draw() {
    std::lock_guard<std::mutex> lock(mMutex);
    for (const auto& log : mLogs) {
        ImGui::TextColored(ImVec4(0.5f, 0.5f, 0.5f, 1.0f), "%s", log.time.c_str());
        ImGui::SameLine();
        ImGui::TextColored(log.color, "%s", log.msg.c_str());
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 20)
        ImGui::SetScrollHereY(1.0f);
}


ServerDashboard::ServerDashboard(ServerConfig config): mConfig(std::move(config)) {

}

ServerDashboard::~ServerDashboard() {
    stop();
}

void ServerDashboard::initialize() {
}

void ServerDashboard::applyTheme() {

}

void ServerDashboard::start() {
    if (!mConfig.isValid()) {
        mLogger.add("Invalid server configuration", ImVec4(1,0,0,1));
        return;
    }

    if (!mCtx.start(mConfig)) {
        mLogger.add("Failed to start server", ImVec4(1,0,0,1));
        return;
    }


    mIsRunning = true;
    mStartTime = std::chrono::steady_clock::now();
}


void ServerDashboard::stop() {
    if (!mIsRunning) return;

    mCtx.stop(mConfig.port);

    mLogger.add("Server stopped on port " + std::to_string(mConfig.port), ImVec4(1,0.6f,0,1));

    mIsRunning = false;
}

void ServerDashboard::update(){

}

void ServerDashboard::render() {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);
    ImGui::Begin("ServerDashboard", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize);

    // --- Top Stats Bar ---
    renderStatsBar();
    ImGui::Separator();

    // --- Split View: Clients (Left) | Logs (Right) ---
    ImGui::Columns(2, "MainColumns", false); // No border between columns
    ImGui::SetColumnWidth(0, 300); // Fixed width for controls

    // Left Column: Controls & Clients
    ImGui::BeginChild("LeftCol", ImVec2(0, 0), true);
    renderControls();
    ImGui::Spacing();
    renderClientList();
    ImGui::EndChild();

    ImGui::NextColumn();

    // Right Column: Logs
    ImGui::BeginChild("RightCol", ImVec2(0,0), true);
    ImGui::Text("System Logs");
    ImGui::Separator();
    ImGui::BeginChild("LogScroll", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
    mLogger.draw();
    ImGui::EndChild();
    ImGui::EndChild();

    // ImGui::EndColumns();
    ImGui::End();
}

void ServerDashboard::renderStatsBar() {
    // Calculate Uptime
    std::string uptime = "0s";
    if (mIsRunning) {
        auto now = std::chrono::steady_clock::now();
        auto sec = std::chrono::duration_cast<std::chrono::seconds>(now - mStartTime).count();
        int h = sec / 3600;
        int m = (sec % 3600) / 60;
        int s = sec % 60;
        char buf[64];
        snprintf(buf, 64, "%02d:%02d:%02d", h, m, s);
        uptime = buf;
    }

    ImGui::Text("STATUS:"); ImGui::SameLine();
    if (mIsRunning) ImGui::TextColored(ImVec4(0, 1, 0, 1), "RUNNING");
    else ImGui::TextColored(ImVec4(1, 0, 0, 1), "STOPPED");

    ImGui::SameLine(150);
    ImGui::Text("Port: %d", mConfig.port);

    ImGui::SameLine(300);
    ImGui::Text("Clients: %zu", mCtx.sessions()->getCount());

    ImGui::SameLine(450);
    ImGui::Text("Uptime: %s", uptime.c_str());
}

void ServerDashboard::renderControls() {
    static char certBuf[256];
    static char keyBuf[256];
    static bool init = false;

    if (!init) {
        std::strncpy(certBuf, mConfig.certFile.c_str(), sizeof(certBuf));
        std::strncpy(keyBuf,  mConfig.keyFile.c_str(),  sizeof(keyBuf));
        init = true;
    }




    ImGui::Text("SERVER CONTROL");
    ImGui::Separator();
    
    ImGui::BeginChild("ControlPanel", ImVec2(0, 250), false, ImGuiWindowFlags_NoScrollbar);
    bool secure = (mConfig.mode == net::server::ServerMode::Secure);
    if (ImGui::Checkbox("Secure (TLS)", &secure)) {
        mConfig.mode = secure
            ? net::server::ServerMode::Secure
            : net::server::ServerMode::Plain;
    }

    if (mConfig.mode == net::server::ServerMode::Secure) {
        ImGui::InputText("Cert File", certBuf, sizeof(certBuf));
        ImGui::InputText("Key File",  keyBuf,  sizeof(keyBuf));
    }


    if (!mIsRunning) {
        ImGui::TextDisabled("Configuration");
        ImGui::PushItemWidth(-1);
        
        int port = mConfig.port;
        if (ImGui::InputInt("Port", &port)) {
            if (port > 0 && port <= 65535)
                mConfig.port = static_cast<uint16_t>(port);
        }

    if (ImGui::Button("Start Server")) {
        mConfig.certFile = certBuf;
        mConfig.keyFile  = keyBuf;
        start();
    }

    } else {
        ImGui::TextColored(ImVec4(0, 0.8f, 1, 1), "Active on Port %d", mConfig.port);

        if (ImGui::Button("Stop Server", ImVec2(-1, 30))) {
            stop();
        }
        ImGui::Separator();
        
        // Broadcast
        ImGui::Text("System Broadcast");
        ImGui::PushItemWidth(-50);
        ImGui::InputText("##bcast", mBroadcastBuf, sizeof(mBroadcastBuf));
        ImGui::PopItemWidth();
        ImGui::SameLine();
        if (ImGui::Button("Send", ImVec2(-1, 0))) {
            if (strlen(mBroadcastBuf) > 0) {
                json body = {
                    {"event", "public_message"}, {"from_uid", 0}, 
                    {"from_name", "SYSTEM"}, {"text", std::string(mBroadcastBuf)}
                };
                mCtx.sessions()->broadcast(Message::makePush(body));
                mLogger.add("Broadcast: " + std::string(mBroadcastBuf), ImVec4(1, 1, 0, 1));
                mBroadcastBuf[0] = 0;
            }
        }
    }
    ImGui::EndChild();
}

void ServerDashboard::renderClientList() {
    ImGui::Text("CONNECTED CLIENTS (%zu)", mCtx.sessions()->getCount());
    ImGui::Separator();
    
    ImGui::BeginChild("ClientListScroll", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);

    auto ids = mCtx.sessions()->listIds();
    if (ids.empty()) {
        ImGui::TextDisabled("Awaiting connections...");
    }

    for (auto uid : ids) {
        std::string name = mCtx.sessions()->getName(uid);
        std::string label = name + " (UID: " + std::to_string(uid) + ")";

        // Ensure we draw the selectable item before the context menu logic
        if (ImGui::Selectable(label.c_str())) {
            // Optional: display client info in a modal here
        }

        // Context Menu (Right-Click Kick)
        if (ImGui::BeginPopupContextItem()) {
            if (ImGui::MenuItem("Kick User")) {
                auto s = mCtx.sessions()->get(uid);
                if (s) { // FIX 1: Null check
                    mLogger.add("Kicking UID: " + std::to_string(uid) + " (" + name + ")", ImVec4(1, 0, 0, 1));
                    s->close();
                } else {
                    mLogger.add("Error: Could not find session for UID " + std::to_string(uid), ImVec4(1, 0, 0, 1));
                }
            }
            ImGui::EndPopup();
        }
    }
    ImGui::EndChild();
}

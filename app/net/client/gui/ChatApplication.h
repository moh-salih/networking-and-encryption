#pragma once
#include <imgui.h>

#include "app/net/common/CoreApplication.h"
#include "app/net/client/common/models.h"
#include "app/net/client/common/ClientAppContext.h"
#include "app/net/client/common/ClientArgs.h"

class ChatApplication: public IApplication{
public:
    ChatApplication() = default;
    ~ChatApplication() = default;

    
    // IApplication interface
    void render() override;

    void update() override;

    void applyTheme() override;

    void initialize() override;


    void setInitialArgs(const app::client::ClientArgs& args);

private:
    void renderLoginScreen();
    void renderConnectingScreen();
    void renderChatScreen();
    void renderMessages();
    void startConnection();
    void sendMessage();
private:
    enum class State { Login, Connecting, MainUI };
    State mState {State::Login};

    ClientAppContext mCtx;




    // --- UI Buffers & State ---
    char mHostBuf[128] = "127.0.0.1";
    char mPortBuf[16] = "12345";
    char mLoginNameBuf[100] = {}; // Initialized empty
    char mChatMsgBuf[512] = {};
    std::string mStatusMsg;
    ImVec4 mStatusColor = ImVec4(1,1,1,1);
    bool mUseSecure = false;

    // --- Chat Data ---
    std::vector<ChatMessage> mMessages;
    std::unordered_map<uint32_t, User> mUsers;
    uint32_t mSelectedUid = 0; // 0 = Public Chat
    
};
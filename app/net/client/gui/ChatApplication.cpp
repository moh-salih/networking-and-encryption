#include "app/net/client/gui/ChatApplication.h"
#include "app/net/client/gui/theme.h"


void ChatApplication::initialize() {
    
    mCtx.events.onConnected = [this] {
        // optional UI hook
    };

    mCtx.events.onLoginSuccess = [this] {
        mState  = State::MainUI;
    };

    mCtx.events.onError = [this](const std::string& err) {
        mStatusMsg = err;
        mStatusColor = ImVec4(1,0.3f,0.3f,1);
        mState = State::Login;
    };

    mCtx.events.onDisconnected = [this] {
        mState = State::Login;
        mStatusMsg = "Disconnected.";
        mUsers.clear();
        mMessages.clear();
    };

    mCtx.events.onStateUpdated = [this] {
        // pull fresh state
        mUsers    = mCtx.users();
        mMessages = mCtx.messages();
    };
}

void ChatApplication::update(){
    mCtx.poll();
}

void ChatApplication::setInitialArgs(const app::client::ClientArgs& args) {
    std::snprintf(mHostBuf, sizeof(mHostBuf), "%s", args.host.c_str());
    std::snprintf(mPortBuf, sizeof(mPortBuf), "%u", args.port);
    std::snprintf(mLoginNameBuf, sizeof(mLoginNameBuf), "%s", args.name.c_str());
    mUseSecure = args.secure;
}

void ChatApplication::startConnection() {
    if (strlen(mLoginNameBuf) < 1) {
        mStatusMsg = "Please enter a username.";
        mStatusColor = ImVec4(1, 0.5f, 0.5f, 1);
        return;
    }

    try {
        net::client::ClientConfig cfg;
        cfg.host = mHostBuf;
        cfg.port = static_cast<uint16_t>(std::stoul(mPortBuf));
        cfg.mode = mUseSecure
            ? net::client::ClientMode::Secure
            : net::client::ClientMode::Plain;

        mCtx.connect(cfg, mLoginNameBuf);

        mState = State::Connecting;
        mStatusMsg = "Connecting...";
        mStatusColor = ImVec4(1,1,1,1);
    }
    catch (const std::exception& e) {
        mState = State::Login;
        mStatusMsg = e.what();
        mStatusColor = ImVec4(1, 0.3f, 0.3f, 1);
    }
}



void ChatApplication::render(){

    ImGui::SetNextWindowPos({0, 0});
    ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize);

    ImGui::Begin("MainWrapper", nullptr, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove);

    switch (mState){
        case State::Login: renderLoginScreen(); break;
        case State::Connecting: renderConnectingScreen(); break;
        case State::MainUI: renderChatScreen(); break;
    }

    ImGui::End();
}

void ChatApplication::applyTheme(){
    ChatThemes::GitHubLight();
}

void CenterWindow(const ImVec2& size) {
    auto windowSize = ImGui::GetIO().DisplaySize;
    ImGui::SetCursorPos(ImVec2(
        (windowSize.x - size.x) * 0.5f,
        (windowSize.y - size.y) * 0.5f
    ));
}

void ChatApplication::renderLoginScreen() {
    ImVec2 childSize(400, 320);
    CenterWindow(childSize);

    ImGui::BeginChild("ConnectionSetup", childSize, true, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoTitleBar);

    ImGui::SetWindowFontScale(1.2f);
    ImGui::Text("Chat Login");
    ImGui::SetWindowFontScale(1.0f);
    ImGui::Separator();
    ImGui::Spacing();

    // Connection Settings
    ImGui::TextDisabled("Server Settings");
    ImGui::PushItemWidth(-1);
    ImGui::InputTextWithHint("##host", "Host (e.g. 127.0.0.1)", mHostBuf, sizeof(mHostBuf));
    ImGui::InputTextWithHint("##port", "Port (e.g. 12345)", mPortBuf, sizeof(mPortBuf));
    ImGui::PopItemWidth();
    ImGui::Spacing();

    // Security Option
    ImGui::Checkbox("Secure (TLS)", &mUseSecure);
    ImGui::Spacing();

    // User Profile
    ImGui::TextDisabled("User Profile");
    ImGui::PushItemWidth(-1);
    bool enter = ImGui::InputTextWithHint("##username", "Username", mLoginNameBuf, sizeof(mLoginNameBuf), ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::PopItemWidth();
    
    ImGui::Spacing();
    
    // Status Message
    if (!mStatusMsg.empty()) {
        ImGui::TextColored(mStatusColor, "%s", mStatusMsg.c_str());
        ImGui::Spacing();
    } else {
        ImGui::Spacing(); 
        ImGui::Spacing(); 
    }

    // Action Button
    if (ImGui::Button("Connect & Login", ImVec2(-1, 40)) || enter) {
        startConnection();
    }

    ImGui::EndChild();
}

void ChatApplication::renderConnectingScreen() {
    ImVec2 childSize(400, 100);
    CenterWindow(childSize);
    ImGui::BeginChild("Connecting", childSize, true);
    ImGui::Text("Connecting to server...");
    ImGui::TextColored(ImVec4(0.4f, 0.8f, 1.0f, 1.0f), "Attempting connection to %s:%s", mHostBuf, mPortBuf);
    ImGui::EndChild();
}

void ChatApplication::renderChatScreen() {
    // Determine the fixed height for the input area (e.g., 50px + padding)
    const float INPUT_HEIGHT = 50.0f;

    // --- User List Sidebar ---
    ImGui::BeginChild("Sidebar", ImVec2(200, 0), true);

    ImGui::TextDisabled("CHANNELS");
    if (ImGui::Selectable(" # Public Chat", mSelectedUid == 0)) {
        mSelectedUid = 0;
    }

    ImGui::Spacing();
    ImGui::TextDisabled("DIRECT MESSAGES");

    for (auto& [uid, user] : mUsers) {
        if (uid == mCtx.myUid()) continue;

        std::string label = user.name;

        // Unread Badge logic
        bool hasUnread = (user.unreadCount > 0);
        if (hasUnread) {
            label += " (" + std::to_string(user.unreadCount) + ")";
            ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 0.8f, 0.2f, 1.0f));
        }

        bool isSelected = (mSelectedUid == uid);
        if (ImGui::Selectable(label.c_str(), isSelected)) {
            mSelectedUid = uid;
            user.unreadCount = 0;
        }

        if (hasUnread) ImGui::PopStyleColor();
    }

    ImGui::EndChild();
    ImGui::SameLine();

    // --- Chat Area (Right Hand Side) ---
    ImGui::BeginGroup();

    // 1. Header (Fixed Height)
    ImGui::BeginChild("Header", ImVec2(0, 40), true);
    if (mSelectedUid == 0)
        ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "Public Chat");
    else
        ImGui::TextColored(ImVec4(0.6f, 0.8f, 1.0f, 1.0f), "Private Chat with %s", mUsers[mSelectedUid].name.c_str());
    ImGui::EndChild();

    // 2. History (Scrollable, takes up remaining space)
    // The negative value makes the history window take all space up to the Input window below.
    ImGui::BeginChild("History", ImVec2(0, -(INPUT_HEIGHT + ImGui::GetStyle().ItemSpacing.y)), true);
    renderMessages();
    ImGui::EndChild();

    // 3. Input (Fixed Height, NOT Scrollable)
    ImGui::BeginChild("Input", ImVec2(0, INPUT_HEIGHT), false); // Fixed height, non-scrolling
    ImGui::PushItemWidth(-70);
    bool sent = ImGui::InputText("##msg", mChatMsgBuf, sizeof(mChatMsgBuf), ImGuiInputTextFlags_EnterReturnsTrue);
    ImGui::PopItemWidth();
    ImGui::SameLine();
    if (ImGui::Button("Send", ImVec2(-1, 0)) || sent) {
        sendMessage();
    }
    ImGui::SetItemDefaultFocus();
        if (sent) ImGui::SetKeyboardFocusHere(-1);
    ImGui::EndChild();

    ImGui::EndGroup();
}

void ChatApplication::renderMessages() {
    float winWidth = ImGui::GetWindowWidth();
    float wrapWidth = winWidth * 0.75f;
    ImGuiStyle& st = ImGui::GetStyle();

    // Dependencies on current theme colors
    const ImVec4 themeAccent = ImGui::GetStyleColorVec4(ImGuiCol_Button);
    const ImVec4 textCol = ImGui::GetStyleColorVec4(ImGuiCol_Text);
    const ImVec4 borderCol = ImGui::GetStyleColorVec4(ImGuiCol_Border);
    const ImVec4 frameBg = ImGui::GetStyleColorVec4(ImGuiCol_FrameBg);

    for (const auto& m : mMessages) {
        // Filtering
        if (mSelectedUid == 0 && m.isPrivate) continue;
        if (mSelectedUid != 0 && (!m.isPrivate || (m.uid != mSelectedUid && m.uid != mCtx.myUid()))) continue;

        bool mine = (m.uid == mCtx.myUid());
        
        ImVec4 bubbleCol;
        if (mine) {
            // Use a lightened version of the theme's blue accent for outgoing messages
            bubbleCol = m.isPrivate ? ImVec4(0.88f, 0.82f, 0.96f, 1.00f) : ImVec4(themeAccent.x, themeAccent.y, themeAccent.z, 0.15f);
        } else {
            // Use the theme's FrameBg (light gray/white) for incoming messages
            bubbleCol = m.isPrivate ? ImVec4(0.94f, 0.92f, 1.00f, 1.00f) : frameBg;
        }

        // 1. Calculate Sizes
        ImVec2 txtSize = ImGui::CalcTextSize(m.text.c_str(), nullptr, false, wrapWidth);
        float bubbleW = txtSize.x + st.FramePadding.x * 4.0f;
        float bubbleH = txtSize.y + st.FramePadding.y * 2.5f;
        if (bubbleW < 100) bubbleW = 100; 

        // 2. Alignment logic
        float cursorX = mine ? (winWidth - bubbleW - st.WindowPadding.x - 25.0f) : st.WindowPadding.x;
        ImGui::SetCursorPosX(cursorX);

        // 3. Render Name Tag
        if (mine) {
            ImGui::TextDisabled("You");
        } else {
            ImGui::TextColored(themeAccent, "%s", m.fromName.c_str());
        }

        // 4. Render Bubble
        ImGui::SetCursorPosX(cursorX);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 12.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 8));
        ImGui::PushStyleColor(ImGuiCol_ChildBg, bubbleCol);
        ImGui::PushStyleColor(ImGuiCol_Text, textCol);
        ImGui::PushStyleColor(ImGuiCol_Border, borderCol);

        std::string bubbleId = "msg_" + std::to_string(reinterpret_cast<uintptr_t>(&m));
        ImGui::BeginChild(bubbleId.c_str(), ImVec2(bubbleW, bubbleH), true, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);
        ImGui::TextWrapped("%s", m.text.c_str());
        ImGui::EndChild();

        ImGui::PopStyleColor(3);
        ImGui::PopStyleVar(2);
        ImGui::Spacing();
        ImGui::Spacing();
    }

    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 100)
        ImGui::SetScrollHereY(1.0f);
}

void ChatApplication::sendMessage() {
    if (strlen(mChatMsgBuf) == 0) return;

    if (mSelectedUid == 0) {
        mCtx.sendPublic(mChatMsgBuf);
    } else {
        mCtx.sendPrivate(mSelectedUid, mChatMsgBuf);
    }

    mChatMsgBuf[0] = 0;
}
#pragma once

#include <boost/asio.hpp>
#include <thread>
#include <mutex>
#include <unordered_map>
#include <functional>
#include <future>
#include <atomic>

#include "net/core/IClient.h"
#include "net/protocol/Message.h"
#include "net/protocol/MessageType.h"
#include "net/protocol/Json.h"
#include "net/core/ITransport.h"

namespace net::client {

    class Client : public net::core::IClient {
    public:
        enum class RunMode{ Threaded, Manual};

        explicit Client(boost::asio::io_context& io, std::unique_ptr<ITransport> transport, RunMode mode = RunMode::Threaded);
        virtual ~Client();

        // IClient
        // Client owns transport connection lifecycle
        bool connect(const std::string& host, uint16_t port) override;
        void close() override;
        bool isRunning() const override;
        void poll() override;

        void requestAsync(const std::string& method, const json& params, ResponseCallback cb) override;

        json request(const std::string& method, const json& params = json::object()) override;

        void onConnect(VoidCallback handler) override;
        void onDisconnect(VoidCallback handler) override;
        void onError(ErrorCallback handler) override;
        void onPush(PushHandler handler) override;

    private:
        // IO Networking helpers
        void readHeader();
        void readBody(size_t payloadSize);
        void writeMessage(const net::protocol::Message& message);
        void handleMessage(const net::protocol::Message& message);


        void stopIo();

        
        void handleConnectResult(const boost::system::error_code& ec);
        void handleHeaderRead(const boost::system::error_code& ec);
        void handleBodyRead(const boost::system::error_code& ec, const std::vector<uint8_t>& payload);

        bool handleIoError(const boost::system::error_code& ec);
    private:
        // Transport
        std::unique_ptr<ITransport> mTransport;
        boost::asio::io_context& mIoContext;
        std::unique_ptr<boost::asio::executor_work_guard<boost::asio::io_context::executor_type>> mWorkGuard;

        // State
        RunMode mRunMode;
        std::atomic<bool> mIsRunning{false};
        std::vector<uint8_t> mReadBuffer;

        std::array<uint8_t, net::protocol::kHeaderSize> mHeaderBuf;

        // Threading & callbacks
        std::thread mThread;
        mutable std::mutex mCallbackMutex;
        std::atomic<uint32_t> mNextRequestId{1};
        std::unordered_map<uint32_t, ResponseCallback> mResponseCallbacks;
        

        // Event handlers
        VoidCallback mConnectCallback;
        VoidCallback mDisconnectCallback;
        ErrorCallback mErrorCallback;
        PushHandler mPushCallback;
    };

} // namespace net::client

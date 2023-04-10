#pragma once
#include "../../IPEndpoint.h"
#include "../../SocketHandle.h"
#include "../../PacketBuffer.h"
#include <functional>
#include <chrono>
#define FRAGMENT_SIZE 1400
namespace Network{
    enum class SocketType{
        RawIPv4, RawIPv6, ManagedIPv4, ManagedIPv6
    };
    enum class ReceiveState{
        ProcessSize, ProcessData
    };


    class Socket {
    public:
        Socket() = default;
        Socket(SocketHandle handle,const IPEndpoint& endpoint, SocketType type);
        bool open(SocketType type);
        void close();
        inline bool isClosed() const {return mHandle == UNDEFINED_SOCKET;}
        bool bind(IPEndpoint& endpoint) const;
        void connect( IPEndpoint& endpoint, const std::function<void(bool)>& callback);
        void setConnectionTimeout(int timeout){ mConnectionTimeout = timeout;}
        bool reconnect();
        void update();
        const IPEndpoint& getEndpoint() const {return mEndpoint;}
        PacketBuffer Rx;
        PacketBuffer Tx;
    private:
        SocketHandle mHandle = UNDEFINED_SOCKET;
        SocketType mType = SocketType::ManagedIPv4;
        std::function<void(bool)> mConnectionCallback;
        bool mConnected = false, mConnecting = false;
        std::chrono::time_point<std::chrono::steady_clock> mConnectingStart;
        int mConnectionTimeout = 10;
        IPEndpoint mEndpoint;
        PollFD mPollFD{};
        int mSentBytes = 0;
        std::vector<uint8_t> mSendBuffer;
        int mReceivedBytes = 0;
        std::vector<uint8_t> mReceiveBuffer{};
        ReceiveState mReceiveState = ReceiveState::ProcessSize;
    };
}

#pragma once
#include "../RawSocket.h"
#include "../Packet.h"
#include "../PacketBuffer.h"
#include <functional>
#include <chrono>

#define FRAGMENT_SIZE 1400
namespace Network {
    enum class SocketType {
        Raw, Managed
    };
    enum class ReceiveState {
        ProcessSize, ProcessData
    };
    class Socket : public RawSocket {
    public:
        Socket(SocketType type) : RawSocket(SocketProtocol::TCP), mType(type) {};
        Socket(SocketHandle handle, const IPEndpoint& endpoint, SocketType type);
        void close() override;
        void connect(IPEndpoint& endpoint, const std::function<void(bool)>& callback);
        void setConnectionTimeout(int timeout) { mConnectionTimeout = timeout; }
        bool reconnect();
        void update() override;
        const IPEndpoint& getEndpoint() const { return mEndpoint; }
        PacketBuffer<Packet> Rx;
        PacketBuffer<Packet> Tx;
    private:
        void onEvent(RawSocketEvent event) override;
        SocketType mType = SocketType::Managed;
        std::function<void(bool)> mConnectionCallback;
        bool mConnected = false, mConnecting = false;
        std::chrono::time_point<std::chrono::steady_clock> mConnectingStart;
        int mConnectionTimeout = 10;
        IPEndpoint mEndpoint;
        int mSentBytes = 0;
        std::vector<uint8_t> mSendBuffer;
        int mReceivedBytes = 0;
        std::vector<uint8_t> mReceiveBuffer{};
        ReceiveState mReceiveState = ReceiveState::ProcessSize;
    };
}

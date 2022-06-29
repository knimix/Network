#pragma once
#include "../Socket.h"

namespace Network {
    class TCPSocket : public Socket {
    public:
        explicit TCPSocket(SocketHandle handle = UNDEFINED_SOCKET) : Socket(SocketType::TCP, handle) {}
        bool Connect(const Endpoint& endpoint);
        bool Reconnect(const Endpoint& endpoint);
        bool Reconnect();
        bool Listen();
        bool Accept(Socket& socket) const;
        bool Send(void* data, size_t size, size_t& bytesSent) const;
        bool Receive(void* data, size_t size, size_t& bytesReceived) const;
    };
}
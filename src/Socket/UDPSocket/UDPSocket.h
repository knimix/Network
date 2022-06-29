#pragma once
#include "../Socket.h"

namespace Network {
    class UDPSocket : public Socket {
    public:
        explicit UDPSocket(SocketHandle handle = UNDEFINED_SOCKET) : Socket(SocketType::UDP, handle) {}
        bool Send(void* data, size_t size, size_t& bytesSent, const Endpoint& destination) const;
        bool Receive(void* data, size_t size, size_t& bytesReceived, Endpoint& from) const;
    };
}
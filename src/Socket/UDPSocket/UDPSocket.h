#pragma once
#include "../Socket.h"

namespace Network{
    class UDPSocket : public Socket{
    public:
        explicit UDPSocket(SocketHandle handle = UNDEFINED_SOCKET) : Socket(SocketType::UDP, handle) {}
        bool Send(const std::vector<char>& buffer, int& bytesSent, const Endpoint& destination) const;
        bool Send(const std::vector<char>& buffer, const Endpoint& destination) const;
        bool Receive(std::vector<char>& buffer, int size, Endpoint& from) const;
    };

}
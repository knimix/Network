#pragma once
#include "../Socket.h"

namespace Network{
    class TCPSocket : public Socket{
    public:
        explicit TCPSocket(SocketHandle handle = UNDEFINED_SOCKET) : Socket(SocketType::TCP, handle) {}
        bool Connect(const Endpoint& endpoint);
        bool Reconnect(const Endpoint& endpoint);
        bool Reconnect();
        bool Listen();
        bool Accept(Socket& socket) const;
        bool Send(const std::vector<char>& buffer, int& bytesSent) const;
        bool Send(const std::vector<char>& buffer) const;
        bool Receive(std::vector<char>& buffer, int size) const;
        uint16_t GetRemotePort() const;
    };

}
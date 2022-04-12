#pragma once
#include "../SocketHandle.h"
#include "../Endpoint/Endpoint.h"
#include "../Event.h"

struct pollfd;

namespace Network{

    enum class SocketType{
        TCP,UDP
    };

    class Socket {
    public:
        Socket();
        explicit Socket(SocketType type, SocketHandle handle = UNDEFINED_SOCKET);
        ~Socket();
        bool Create();
        bool Close();
        bool Bind(const Endpoint& endpoint);
        bool SetBlocking(bool blocking) const;
        bool PollEvents(Event& event);
        inline SocketHandle GetSocketHandle() const {return m_Handle;}
        inline Endpoint& GetEndpoint() {return m_Endpoint;}
        pollfd* PollFD = nullptr;

    protected:
        SocketHandle m_Handle = UNDEFINED_SOCKET;
        bool m_Listen = false;
        bool m_Connected = false;
        Endpoint m_Endpoint = {};
        SocketType m_SocketType = SocketType::TCP;
    };
}

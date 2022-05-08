#pragma once
#include "../SocketHandle.h"
#include "../Endpoint/Endpoint.h"
#include "../Event.h"
#include <functional>

namespace Network{

    enum class SocketType{
        TCP,UDP
    };

    class Socket {
    public:
        Socket() = default;
        explicit Socket(SocketType type, SocketHandle handle = UNDEFINED_SOCKET);
        bool Create();
        void Close();
        bool Bind(const Endpoint& endpoint);
        bool SetBlocking(bool blocking);
        bool PollEvents();
        inline SocketHandle GetSocketHandle() const {return m_Handle;}
        inline Endpoint& GetEndpoint() {return m_Endpoint;}
        inline void SetEventCallback(const std::function<void(Socket&,const Event&)>& callback){m_EventCallback = callback;}
        inline bool IsClosed() const{return m_Handle == UNDEFINED_SOCKET;}
    protected:
        std::function<void(Socket&,const Event&)> m_EventCallback;
        SocketHandle m_Handle = UNDEFINED_SOCKET;
        bool m_Listen = false;
        bool m_Connected = false;
        bool m_Connecting = false;
        bool m_Blocking = false;
        Endpoint m_Endpoint = {};
        SocketType m_SocketType = SocketType::TCP;
        PollFD m_PollFD{};
    };
}

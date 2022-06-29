#pragma once
#include <functional>
#include "../SocketHandle.h"
#include "../Endpoint/Endpoint.h"
#include "../Events.h"

namespace Network {
    enum class SocketType {
        TCP, UDP
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
        inline SocketHandle GetSocketHandle() const { return m_Handle; }
        inline Endpoint& GetEndpoint() { return m_Endpoint; }
        inline void SetEventCallback(const std::function<void(SocketEvent)>& callback) { m_EventCallback = callback; }
        inline bool IsClosed() const { return m_Handle == UNDEFINED_SOCKET; }
        uint16_t GetRemotePort() const;
    protected:
        std::function<void(SocketEvent)> m_EventCallback;
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

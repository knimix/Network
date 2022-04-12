#include <iostream>
#include "Socket.h"
#include "../Core.h"

Network::Socket::Socket(Network::SocketType type, SocketHandle handle) : m_SocketType(type), m_Handle(handle) {
    m_PollFD.fd = m_Handle;
}

bool Network::Socket::Create() {
    if (m_Handle != UNDEFINED_SOCKET) {
        return false;
    }
    if (m_SocketType == SocketType::TCP) {
        m_Handle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    } else {
        m_Handle = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }
    if (m_Handle == UNDEFINED_SOCKET) {
        return false;
    }
    if (m_SocketType == SocketType::TCP) {
        int value = true;
        if (setsockopt(m_Handle, IPPROTO_TCP, TCP_NODELAY, (const char *) &value, sizeof(value))) {
            return false;
        }
    }
    m_PollFD.fd = m_Handle;
    return true;
}

bool Network::Socket::Close() {
    if (m_Handle == UNDEFINED_SOCKET) {
        return false;
    }
    int result = CloseSocket(m_Handle);
    if (result != 0) {
        return false;
    }
    m_Handle = UNDEFINED_SOCKET;
    return true;
}

bool Network::Socket::Bind(const Network::Endpoint &endpoint) {
    m_Endpoint = endpoint;
    sockaddr_in address = endpoint.GetSockAddress();
    int result = bind(m_Handle, (sockaddr *) &address, sizeof(sockaddr_in));
    return result == 0;
}

bool Network::Socket::SetBlocking(bool blocking) const {
#ifdef _WIN32
    unsigned long mode = blocking ? 0 : 1;
    return (ioctlsocket(m_Handle, FIONBIO, &mode) == 0);
#else
    int flags = fcntl(m_Handle, F_GETFL, 0);
    if (flags == -1) return false;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(m_Handle, F_SETFL, flags) == 0);
#endif
}

bool Network::Socket::PollEvents(Network::Event &event) {
    if (m_Handle == UNDEFINED_SOCKET) {
        return false;
    }
    pollfd fd = *(pollfd*)&m_PollFD;
    event = Event::None;
    if (Poll(&fd, 1, 0) > 0) {
        if (fd.revents & POLLERR) {
            if (!m_Connected) {
                Close();
                event = Event::OnConnectFail;
                return true;
            }
            Close();
            event = Event::OnDisconnect;
            return true;
        }
        if (fd.revents & POLLHUP) {
            Close();
            event = Event::OnDisconnect;
            return true;
        }
        if (fd.revents & POLLNVAL) {
            Close();
            event = Event::OnDisconnect;
            return true;
        }
        if (fd.revents & POLLRDNORM) {
            if (m_Listen) {
                event = (Event::OnAcceptConnection);
            } else {
                event = Event::OnReceive;
            }
            return true;
        }
        if (fd.revents & POLLWRNORM) {
            if (!m_Connected && m_SocketType == SocketType::TCP) {
                m_Connected = true;
                event = Event::OnConnect;
                return true;
            }
            event = Event::OnSend;
            return true;
        }
    }
    return false;
}







#include "Socket.h"
#include "../Core.h"

Network::Socket::Socket(Network::SocketType type, SocketHandle handle) : m_SocketType(type), m_Handle(handle) {
    m_PollFD.fd = m_Handle;
    m_PollFD.events = POLLRDNORM | POLLWRNORM;
    if(handle != UNDEFINED_SOCKET){
        m_Connected = true;
    }
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
    m_PollFD.events = POLLRDNORM | POLLWRNORM;
    return true;
}

void Network::Socket::Close() {
    if (m_Handle == UNDEFINED_SOCKET) {
        return;
    }
    CloseSocket(m_Handle);
    m_Handle = UNDEFINED_SOCKET;
    if(m_EventCallback){
        m_EventCallback(Event::OnError);
    }
    if(!m_Connected && m_SocketType == SocketType::TCP){
        if(m_EventCallback){
            m_EventCallback(Event::OnConnectFail);
        }
    }
    m_Connected = false;
    m_Listen = false;
}

bool Network::Socket::Bind(const Network::Endpoint &endpoint) {
    m_Endpoint = endpoint;
    sockaddr_in address = endpoint.GetSockAddress();
    int result = bind(m_Handle, (sockaddr *) &address, sizeof(sockaddr_in));
    return result == 0;
}

bool Network::Socket::SetBlocking(bool blocking) {
    m_Blocking = blocking;
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

bool Network::Socket::PollEvents() {
    if (m_Handle == UNDEFINED_SOCKET) {
        return false;
    }
    pollfd fd = *(pollfd*)&m_PollFD;
    if (Poll(&fd, 1, 0) > 0) {
        if (fd.revents & POLLERR) {
            Close();
            return true;
        }
        if (fd.revents & POLLHUP) {
            Close();
            return true;
        }
        if (fd.revents & POLLNVAL) {
            Close();
            return true;
        }
        if (fd.revents & POLLRDNORM) {
            if(m_SocketType == SocketType::TCP){
                char temp;
                if(recv(m_Handle,&temp,sizeof(char),MSG_PEEK) == 0){
                    Close();
                    return true;
                }
            }
            if(!m_EventCallback){
                return true;
            }
            if (m_Listen) {
                m_EventCallback(Event::OnAcceptConnection);
            } else {
                m_EventCallback(Event::OnReceive);
            }
            return true;
        }
        if (fd.revents & POLLWRNORM) {
            if (!m_Connected && m_SocketType == SocketType::TCP && !m_Listen) {
                m_Connected = true;
                if(!m_EventCallback){
                    return true;
                }
                m_EventCallback(Event::OnConnect);
                return true;
            }
            if(!m_EventCallback){
                return true;
            }
            m_EventCallback(Event::OnSend);
            return true;
        }
    }
    return false;
}







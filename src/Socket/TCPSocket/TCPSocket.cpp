#include "TCPSocket.h"
#include "../../Core.h"

bool Network::TCPSocket::Connect(const Network::Endpoint& endpoint) {
    if (m_Handle == UNDEFINED_SOCKET || m_Listen || !endpoint) {
        return false;
    }
    m_Endpoint = endpoint;
    if (m_SocketType == SocketType::TCP) {
        sockaddr_in address = endpoint.GetSockAddress();
        connect(m_Handle, (sockaddr*) &address, sizeof(sockaddr_in));
        m_Connecting = true;
    }
    return true;
}
bool Network::TCPSocket::Reconnect(const Endpoint& endpoint) {
    if (m_Listen || !endpoint) {
        return false;
    }
    if (!IsClosed()) {
        Close();
    }
    if (!Create()) {
        return false;
    }
    if (!SetBlocking(m_Blocking)) {
        return false;
    }
    if (!Connect(endpoint)) {
        return false;
    }
    return true;
}
bool Network::TCPSocket::Reconnect() {
    if (m_Listen || !m_Endpoint) {
        return false;
    }
    if (!IsClosed()) {
        Close();
    }
    if (!Create()) {
        return false;
    }
    if (!SetBlocking(m_Blocking)) {
        return false;
    }
    if (!Connect(m_Endpoint)) {
        return false;
    }
    return true;
}
bool Network::TCPSocket::Listen() {
    m_Listen = true;
    return listen(m_Handle, SOMAXCONN) == 0;
}
bool Network::TCPSocket::Accept(Network::Socket& socket) const {
    if (m_SocketType == SocketType::TCP) {
        sockaddr_in address = {};
        socklen_t len = sizeof(sockaddr_in);
        SocketHandle clientHandle = accept(m_Handle, (sockaddr*) &address, &len);
        if (clientHandle == UNDEFINED_SOCKET) {
            return false;
        }
        socket = Socket(SocketType::TCP, clientHandle);;
        socket.GetEndpoint() = Endpoint((sockaddr*) &address);
        return true;
    }
    return false;
}
bool Network::TCPSocket::Send(void* data, size_t size, size_t& bytesSent) const {
    bytesSent = send(m_Handle, data, size, 0);
    if (bytesSent == SOCKET_ERROR) {
        return false;
    }
    return true;
}
bool Network::TCPSocket::Receive(void* data, size_t size, size_t& bytesReceived) const {
    bytesReceived = recv(m_Handle, data, size, 0);
    if (bytesReceived == 0) {
        return false;
    }
    if (bytesReceived == SOCKET_ERROR) {
        return false;
    }
    return true;
}




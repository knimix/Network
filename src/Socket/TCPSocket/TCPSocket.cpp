#include "TCPSocket.h"
#include "../../Core.h"

void Network::TCPSocket::Connect(const Network::Endpoint &endpoint) {
    m_Endpoint = endpoint;
    if (m_SocketType == SocketType::TCP) {
        sockaddr_in address = endpoint.GetSockAddress();
        connect(m_Handle, (sockaddr *) &address, sizeof(sockaddr_in));
    }
}

bool Network::TCPSocket::Listen() {
    m_Listen = true;
    return listen(m_Handle, SOMAXCONN) == 0;
}

bool Network::TCPSocket::Accept(Network::Socket &socket) const {
    if (m_SocketType == SocketType::TCP) {
        sockaddr_in address = {};
        socklen_t len = sizeof(sockaddr_in);
        SocketHandle clientHandle = accept(m_Handle, (sockaddr *) &address, &len);
        if (clientHandle == UNDEFINED_SOCKET) {
            return false;
        }
        socket = Socket(SocketType::TCP, clientHandle);;
        socket.GetEndpoint() = Endpoint((sockaddr *) &address);
        return true;
    }
    return false;
}

bool Network::TCPSocket::Send(const std::vector<char>& buffer, int &bytesSent) const {
    bytesSent = send(m_Handle, &buffer[0], (int)buffer.size(), 0);
    if (bytesSent == SOCKET_ERROR) {
        return false;
    }
    return true;
}

bool Network::TCPSocket::Send(const std::vector<char> &buffer) const {
    int bytesSent = send(m_Handle, &buffer[0], (int)buffer.size(), 0);
    if (bytesSent == SOCKET_ERROR) {
        return false;
    }
    return true;
}

bool Network::TCPSocket::Receive(std::vector<char>& buffer, int size) const {
    buffer.resize(size);
    int bytesReceived = recv(m_Handle, &buffer[0], size, 0);
    if(bytesReceived == 0){
        return false;
    }
    if (bytesReceived == SOCKET_ERROR) {
        return false;
    }
    buffer.resize(bytesReceived);
    return true;
}

uint16_t Network::TCPSocket::GetRemotePort() const {
    struct sockaddr_in client{};
    int clientSize = sizeof(client);
    getsockname(GetSocketHandle(), (struct sockaddr *) &client, &clientSize);
    return ntohs(client.sin_port);
}

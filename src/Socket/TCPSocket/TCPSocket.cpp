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
    int result = listen(m_Handle, SOMAXCONN);
    m_Listen = true;
    return result == 0;
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
    char tempBuffer[size];
    int bytesReceived = {};
    bytesReceived = recv(m_Handle, tempBuffer, size, 0);
    if(bytesReceived == 0){
        return false;
    }
    if (bytesReceived == SOCKET_ERROR) {
        return false;
    }
    buffer.clear();
    buffer.resize(bytesReceived);
    buffer.assign(tempBuffer, tempBuffer+bytesReceived);
    return true;
}

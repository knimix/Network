#include <iostream>
#include "UDPSocket.h"
#include "../../Core.h"


bool Network::UDPSocket::Send(const std::vector<char>& buffer, int &bytesSent, const Endpoint& destination) const {
    sockaddr_in address = destination.GetSockAddress();
    bytesSent = sendto(m_Handle,  &buffer[0], (int)buffer.size(), 0, (sockaddr*)&address, sizeof(sockaddr));
    if (bytesSent == SOCKET_ERROR) {
        return false;
    }
    return true;
}
bool Network::UDPSocket::Send(const std::vector<char> &buffer, const Network::Endpoint &destination) const {
    sockaddr_in address = destination.GetSockAddress();
    int bytesSent = sendto(m_Handle,  &buffer[0], (int)buffer.size(), 0, (sockaddr*)&address, sizeof(sockaddr));
    if (bytesSent == SOCKET_ERROR) {
        return false;
    }
    return true;
}

bool Network::UDPSocket::Receive(std::vector<char>& buffer, int size, Endpoint& from) const {
    buffer.resize(size);
    sockaddr address{};
    socklen_t length = sizeof(sockaddr);
    size_t bytesReceived = recvfrom(m_Handle, &buffer[0], size, 0,&address,&length);
    if(bytesReceived == 0){
        return false;
    }
    if (bytesReceived == SOCKET_ERROR) {
        return false;
    }
    from = Endpoint((sockaddr*)&address);
    buffer.resize(bytesReceived);
    return true;
}

#include "UDPSocket.h"
#include "../../Core.h"

bool Network::UDPSocket::Send(void* data, size_t size, size_t& bytesSent, const Endpoint& destination) const {
    sockaddr_in address = destination.GetSockAddress();
    bytesSent = sendto(m_Handle, (char*)data, size, 0, (sockaddr*) &address, sizeof(sockaddr));
    if (bytesSent == SOCKET_ERROR) {
        return false;
    }
    return true;
}
bool Network::UDPSocket::Receive(void* data, size_t size, size_t& bytesReceived, Endpoint& from) const {
    sockaddr address{};
    socklen_t length = sizeof(sockaddr);
    bytesReceived = recvfrom(m_Handle,  (char*)data, size, 0, &address, &length);
    if (bytesReceived == SOCKET_ERROR) {
        return false;
    }
    from = Endpoint((sockaddr*) &address);
    return true;
}

#include "Endpoint.h"
#include "../Core.h"
#include <algorithm>
#include <cstring>

Network::Endpoint::Endpoint(sockaddr* address) {
    auto* addressV4 = reinterpret_cast<sockaddr_in*>(address);
    m_Port = ntohs(addressV4->sin_port);
    m_IPBytes.resize(sizeof(uint32_t));
    memcpy(&m_IPBytes[0], &addressV4->sin_addr, sizeof(uint32_t));
    m_IP.resize(16);
    inet_ntop(AF_INET, &addressV4->sin_addr, &m_IP[0], 16);
    m_IP.erase(std::find(m_IP.begin(), m_IP.end(), '\0'), m_IP.end());
    m_Hostname = m_IP;
    m_Success = true;
}
Network::Endpoint::Endpoint(const char* ip, unsigned short port) {
    m_Port = port;
    in_addr address{};
    m_IP.resize(16);
    m_IPBytes.resize(sizeof(uint32_t));
    if (inet_pton(AF_INET, ip, &address)) {
        if (address.s_addr != INADDR_NONE) {
            m_IP = ip;
            m_Hostname = ip;
            memcpy(&m_IPBytes[0], &address.s_addr, sizeof(uint32_t));
            m_Success = true;
            return;
        }
    }
    addrinfo hints{};
    hints.ai_family = AF_INET;
    addrinfo* hostInfo = nullptr;
    if (getaddrinfo(ip, nullptr, &hints, &hostInfo) == 0) {
        auto hostAddress = reinterpret_cast<sockaddr_in*>(hostInfo->ai_addr);
        inet_ntop(AF_INET, &hostAddress->sin_addr, &m_IP[0], 16);
        m_IP.erase(std::find(m_IP.begin(), m_IP.end(), '\0'), m_IP.end());
        m_Hostname = ip;
        uint32_t ipBytes = hostAddress->sin_addr.s_addr;
        memcpy(&m_IPBytes[0], &ipBytes, sizeof(uint32_t));
        m_Success = true;
    }
    freeaddrinfo(hostInfo);
}
sockaddr_in Network::Endpoint::GetSockAddress() const {
    sockaddr_in address{};
    if (m_Success) {
        address.sin_family = AF_INET;
        memcpy(&address.sin_addr, &m_IPBytes[0], sizeof(uint32_t));
        address.sin_port = htons(m_Port);
    }
    return address;
}

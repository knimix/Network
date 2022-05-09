#pragma once
#include <string>
#include <vector>

class sockaddr;
class sockaddr_in;

namespace Network{
    class Endpoint {
    public:
        Endpoint() = default;
        explicit Endpoint(sockaddr* address);
        Endpoint(const char* ip, unsigned  short port);
        inline const std::string& GetHostname() const {return m_Hostname;}
        inline const std::string& GetIP() const {return m_IP;}
        inline const std::vector<uint8_t>& GetIPBytes() const {return m_IPBytes;}
        inline const unsigned short GetPort() const {return m_Port;}
        sockaddr_in GetSockAddress() const;
        explicit operator bool() const{return m_Success;}
    private:
        bool m_Success = false;
        std::string m_Hostname;
        std::string m_IP;
        std::vector<uint8_t> m_IPBytes;
        unsigned short m_Port = 0;
    };

}
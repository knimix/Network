#pragma once
#include <string_view>
#include <string>

class sockaddr_in;
class sockaddr_in6;
namespace Network {
    class IPEndpoint {
    public:
        IPEndpoint() = default;
        IPEndpoint(const std::string_view& address, uint16_t port) : mAddress(address), mPort(port) {};
        explicit IPEndpoint(sockaddr_in* address);
        explicit IPEndpoint(sockaddr_in6* address);
        bool solveIPv4(sockaddr_in* address);
        bool solveIPv6(sockaddr_in6* address);
        inline uint16_t getPort() const { return mPort; }
        inline std::string getIP() const { return mIP; }
        inline std::string getHostname() const { return mHostname; }
        inline std::string getAddress() const { return mAddress; }
        std::string toString() const;
    private:
        uint16_t mPort = 0;
        std::string mIP, mHostname, mAddress;
    };
}
#include <algorithm>
#include <iostream>
#include <cstring>
#include "Core.h"
#include "IPEndpoint.h"

namespace Network {
    IPEndpoint::IPEndpoint(sockaddr_in* address) {
        if (address == nullptr) {
            return;
        }
        mPort = ntohs(address->sin_port);
        mIP.resize(16);
        inet_ntop(AF_INET, &address->sin_addr, &mIP[0], 16);
        mIP.erase(std::find(mIP.begin(), mIP.end(), '\0'), mIP.end());
        mHostname = mIP;
        mAddress = mIP;
    }
    IPEndpoint::IPEndpoint(sockaddr_in6* address) {
        if (address == nullptr) {
            return;
        }
        mPort = ntohs(address->sin6_port);
        mIP.resize(16);
        inet_ntop(AF_INET6, &address->sin6_addr, &mIP[0], 16);
        mIP.erase(std::find(mIP.begin(), mIP.end(), '\0'), mIP.end());
        mHostname = mIP;
        mAddress = mIP;
    }
    bool IPEndpoint::solveIPv4(sockaddr_in* address) {
        in_addr inAddress{};
        if (inet_pton(AF_INET, mAddress.c_str(), &inAddress) == 1) {
            if (address != nullptr) {
                address->sin_family = AF_INET;
                address->sin_port = htons(mPort);
                memcpy(&address->sin_addr, &inAddress.s_addr, sizeof(uint32_t));
            }
            mIP = mAddress;
            mHostname = mAddress;
            return true;
        }
        addrinfo hints{};
        hints.ai_family = AF_INET;
        addrinfo* hostInfo = nullptr;
        if (getaddrinfo(mAddress.c_str(), nullptr, &hints, &hostInfo) == 0) {
            mHostname = mAddress;
            auto hostAddress = reinterpret_cast<sockaddr_in*>(hostInfo->ai_addr);
            if (address != nullptr) {
                address->sin_family = AF_INET;
                address->sin_port = htons(mPort);
                memcpy(&address->sin_addr, &hostAddress->sin_addr, sizeof(uint32_t));
            }
            mIP.resize(16);
            inet_ntop(AF_INET, &hostAddress->sin_addr, &mIP[0], 16);
            mIP.erase(std::find(mIP.begin(), mIP.end(), '\0'), mIP.end());
        } else {
            freeaddrinfo(hostInfo);
            return false;
        }
        freeaddrinfo(hostInfo);
        return true;
    }
    bool IPEndpoint::solveIPv6(sockaddr_in6* address) {
        in6_addr inAddress{};
        if (inet_pton(AF_INET6, mAddress.c_str(), &inAddress) == 1) {
            if (address != nullptr) {
                address->sin6_family = AF_INET6;
                address->sin6_port = htons(mPort);
                memcpy(&address->sin6_addr, &inAddress, 16);
            }
            mIP = mAddress;
            mHostname = mAddress;
            return true;
        }
        addrinfo hints{};
        hints.ai_family = AF_INET6;
        addrinfo* hostInfo = nullptr;
        if (getaddrinfo(mAddress.c_str(), nullptr, &hints, &hostInfo) == 0) {
            mHostname = mAddress;
            auto hostAddress = reinterpret_cast<sockaddr_in6*>(hostInfo->ai_addr);
            if (address != nullptr) {
                address->sin6_family = AF_INET6;
                address->sin6_port = htons(mPort);
                memcpy(&address->sin6_addr, &hostAddress->sin6_addr, sizeof(uint32_t));
            }
            mIP.resize(16);
            inet_ntop(AF_INET6, &hostAddress->sin6_addr, &mIP[0], 16);
            mIP.erase(std::find(mIP.begin(), mIP.end(), '\0'), mIP.end());
        } else {
            freeaddrinfo(hostInfo);
            return false;
        }
        freeaddrinfo(hostInfo);
        return true;
    }
    std::string IPEndpoint::toString() const{
        return mIP + ":" + std::to_string(mPort);
    }
}


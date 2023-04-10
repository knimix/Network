#include "Network.h"

namespace Network {
    bool initialize() {
#ifdef _WIN32
        WSADATA wsaData;
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0) {
        return false;
    }
    if (LOBYTE(wsaData.wVersion) != 2 || HIBYTE(wsaData.wVersion) != 2) {
        return false;
    }
    return true;
#else
        return true;
#endif
    }
    void terminate() {
#ifdef _WIN32
        WSACleanup();
#endif
    }
}
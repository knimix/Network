#include <Network/Network.h>
#include <Network/Core.h>
#include <Util/Platform.h>

namespace Network {
    bool initialize() {
#ifdef PLATFORM_WINDOWS
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
#ifdef PLATFORM_WINDOWS
        WSACleanup();
#endif
    }
}
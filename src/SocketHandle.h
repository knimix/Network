#pragma once
#include <cstdint>
#include <Util/Platform.h>

#ifdef PLATFORM_WINDOWS
typedef uint64_t SocketHandle;
#define UNDEFINED_SOCKET (SocketHandle)~0
#endif
#ifdef PLATFORM_UNIX
typedef int SocketHandle;
#define UNDEFINED_SOCKET (SocketHandle)~0
#define SOCKET_ERROR -1
#endif
struct PollFD {
    SocketHandle fd = UNDEFINED_SOCKET;
    short events = 0;
    short revents = 0;
};


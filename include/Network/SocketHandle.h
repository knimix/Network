#pragma once
#include "Util/Platform.h"
#include <cstdint>

#ifdef PLATFORM_WINDOWS
typedef uint64_t SocketHandle;
#define UNDEFINED_SOCKET (SocketHandle)~0
#endif
#if defined(PLATFORM_LINUX) || defined(PLATFORM_ANDROID)
typedef int SocketHandle;
#define UNDEFINED_SOCKET (SocketHandle)~0
#define SOCKET_ERROR -1
#endif
struct PollFD {
    SocketHandle fd = UNDEFINED_SOCKET;
    short events = 0;
    short revents = 0;
};


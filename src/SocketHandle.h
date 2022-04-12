#pragma once

#include <cstdint>

#ifdef _WIN32
typedef uint64_t SocketHandle;
#define UNDEFINED_SOCKET (SocketHandle)~0
#endif

#ifdef __unix__
typedef int SocketHandle;
#define UNDEFINED_SOCKET (SocketHandle)~0
#define SOCKET_ERROR -1
#endif

struct PollFD {
    SocketHandle fd = UNDEFINED_SOCKET;
    short events = 0;
    short revents = 0;
};


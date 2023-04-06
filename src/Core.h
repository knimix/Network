#pragma once

#ifdef _WIN32
#include <ws2tcpip.h>
#include <ws2def.h>
#include <winsock2.h>
#define closeSocket(fd) closesocket(fd)
#define poll(x,y,z) WSAPoll(x,y,z)
#endif

#ifdef __unix__
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <fcntl.h>
#include <unistd.h>
#include <poll.h>
#define closeSocket(fd) close(fd)
#endif

#include "RawSocket.h"
#include "../Core.h"

bool Network::RawSocket::open(Network::SocketVersion version) {
    if (!isClosed()) {
        return false;
    }
    mVersion = version;
    int domain, type, proto;
    version == SocketVersion::IPv4 ? domain = AF_INET : domain = AF_INET6;
    mProtocol == SocketProtocol::TCP ? type = SOCK_STREAM : type = SOCK_DGRAM;
    mProtocol == SocketProtocol::TCP ? proto = IPPROTO_TCP : type = IPPROTO_UDP;
    mHandle = socket(domain, type, proto);
    if (isClosed()) {
        return false;
    }
    mPollFD.fd = mHandle;
    mPollFD.events = POLLRDNORM | POLLWRNORM;
    bool blocking = false;
#ifdef _WIN32
    unsigned long mode = blocking ? 0 : 1;
        return (ioctlsocket(mHandle, FIONBIO, &mode) == 0);
#else
    int flags = fcntl(mHandle, F_GETFL, 0);
    if (flags == -1) return false;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(mHandle, F_SETFL, flags) == 0);
#endif
}
void Network::RawSocket::close() {
    ::closeSocket(mHandle);
    mHandle = UNDEFINED_SOCKET;
}
bool Network::RawSocket::bind(Network::IPEndpoint &endpoint) const {
    if (isClosed()) {
        return false;
    }
    if (mVersion == SocketVersion::IPv4) {
        sockaddr_in address{};
        if (!endpoint.solveIPv4(&address)) {
            return false;
        }
        return ::bind(mHandle, (sockaddr *) &address, sizeof(sockaddr_in)) == 0;
    } else {
        sockaddr_in6 address{};
        if (!endpoint.solveIPv6(&address)) {
            return false;
        }
        return ::bind(mHandle, (sockaddr *) &address, sizeof(sockaddr_in6)) == 0;
    }
}
void Network::RawSocket::update() {
    if (isClosed()) {
        return;
    }
    pollfd fd = *(pollfd *) &mPollFD;
    if (poll(&fd, 1, 0) > 0) {
        if (fd.revents & POLLERR) {
            onEvent(RawSocketEvent::Error);
        }
        if (fd.revents & POLLHUP) {
            onEvent(RawSocketEvent::Error);
        }
        if (fd.revents & POLLNVAL) {
            onEvent(RawSocketEvent::Error);
        }
        if (fd.revents & POLLRDNORM) {
            onEvent(RawSocketEvent::Read);
        }
        if (fd.revents & POLLWRNORM) {
            onEvent(RawSocketEvent::Write);
        }
    }
}

#include <Network/RawSocket.h>
#include <Network/Core.h>

namespace Network {
    bool RawSocket::open(SocketVersion version) {
        if (!isClosed()) {
            return false;
        }
        mVersion = version;
        int domain, type, proto;
        if(version == SocketVersion::IPv4){
            domain = AF_INET;
        }else{
            domain = AF_INET6;
        }
        if(mProtocol == SocketProtocol::TCP){
            type = SOCK_STREAM;
            proto = IPPROTO_TCP;
        }else{
            type = SOCK_DGRAM;
            proto = IPPROTO_UDP;
        }
        mHandle = socket(domain, type, proto);
        if (isClosed()) {
            return false;
        }
        mPollFD.fd = mHandle;
        mPollFD.events = POLLRDNORM | POLLWRNORM;
        bool blocking = false;
#ifdef PLATFORM_WINDOWS
        unsigned long mode = blocking ? 0 : 1;
        return (ioctlsocket(mHandle, FIONBIO, &mode) == 0);
#endif
#ifdef PLATFORM_UNIX
        int flags = fcntl(mHandle, F_GETFL, 0);
        if (flags == -1) return false;
        flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
        return (fcntl(mHandle, F_SETFL, flags) == 0);
#endif
    }
    void RawSocket::close() {
        ::closeSocket(mHandle);
        mHandle = UNDEFINED_SOCKET;
    }
    bool RawSocket::bind(IPEndpoint& endpoint) const {
        if (isClosed()) {
            return false;
        }
        if (mVersion == SocketVersion::IPv4) {
            sockaddr_in address{};
            if (!endpoint.solveIPv4(&address)) {
                return false;
            }
            return ::bind(mHandle, (sockaddr*) &address, sizeof(sockaddr_in)) == 0;
        } else {
            sockaddr_in6 address{};
            if (!endpoint.solveIPv6(&address)) {
                return false;
            }
            return ::bind(mHandle, (sockaddr*) &address, sizeof(sockaddr_in6)) == 0;
        }
    }
    void RawSocket::update() {
        if (isClosed()) {
            return;
        }
        pollfd fd = *(pollfd*) &mPollFD;
        if (poll(&fd, 1, 0) > 0) {
            if (fd.revents & POLLERR) {
                onEvent(RawSocketEvent::Error);
            }else if (fd.revents & POLLHUP) {
                onEvent(RawSocketEvent::Error);
            }else if (fd.revents & POLLNVAL) {
                onEvent(RawSocketEvent::Error);
            }else if (fd.revents & POLLRDNORM) {
                onEvent(RawSocketEvent::Read);
            }else if (fd.revents & POLLWRNORM) {
                onEvent(RawSocketEvent::Write);
            }
        }
    }
}
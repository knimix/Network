#include "ServerSocket.h"
#include "../../Core.h"

bool Network::ServerSocket::open(Network::SocketType type) {
    if (!isClosed()) {
        return false;
    }
    mType = type;
    if (mType == SocketType::RawIPv4 || mType == SocketType::ManagedIPv4) {
        mHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    } else {
        mHandle = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
    }
    if(isClosed()){
        return false;
    }
    mPollFD.fd = mHandle;
    mPollFD.events = POLLRDNORM;
    bool blocking = false;
#ifdef _WIN32
    unsigned long mode = blocking ? 0 : 1;
    return (ioctlsocket(mHandle, FIONBIO, &mode) == 0);
#else
    int flags = fcntl(m_Handle, F_GETFL, 0);
    if (flags == -1) return false;
    flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
    return (fcntl(mHandle, F_SETFL, flags) == 0);
#endif
}

void Network::ServerSocket::close() {
    if(isClosed()){
        return;
    }
    closeSocket(mHandle);
    mHandle = UNDEFINED_SOCKET;
}

bool Network::ServerSocket::bind(Network::IPEndpoint &endpoint) const {
    if(isClosed()){
        return false;
    }
    if (mType == SocketType::RawIPv4 || mType == SocketType::ManagedIPv4) {
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

void Network::ServerSocket::listen(int count) const {
    ::listen(mHandle,count);
}

void Network::ServerSocket::update() {
    if (isClosed()) {
        return;
    }
    pollfd fd = *(pollfd *) &mPollFD;
    if (poll(&fd, 1, 0) > 0) {
        if (fd.revents & POLLERR) {
            close();
            return;
        }
        if (fd.revents & POLLHUP) {
            close();
            return;
        }
        if (fd.revents & POLLNVAL) {
            close();
            return;
        }
        if (fd.revents & POLLRDNORM) {
            char temp;
            if (recv(mHandle, &temp, sizeof(char), MSG_PEEK) == 0) {
                close();
                return;
            }
            if(mType == SocketType::RawIPv4 || mType == SocketType::ManagedIPv4){
                sockaddr_in address = {};
                socklen_t len = sizeof(sockaddr_in);
                SocketHandle clientHandle = ::accept(mHandle, (sockaddr*) &address, &len);
                if (clientHandle == UNDEFINED_SOCKET) {
                    return;
                }
                auto socket = std::make_shared<Socket>(clientHandle,IPEndpoint(&address),mType);
                if(mAcceptCallback){
                    mAcceptCallback(socket);
                }
            }else{
                sockaddr_in6 address = {};
                socklen_t len = sizeof(sockaddr_in6);
                SocketHandle clientHandle = ::accept(mHandle, (sockaddr*) &address, &len);
                if (clientHandle == UNDEFINED_SOCKET) {
                    return;
                }
                auto socket = std::make_shared<Socket>(clientHandle,IPEndpoint(&address),mType);
                if(mAcceptCallback){
                    mAcceptCallback(socket);
                }
            }
            return;
        }
    }
}

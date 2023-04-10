#include "ServerSocket.h"
#include "../../Core.h"

namespace Network {
    void ServerSocket::listen(int count) const {
        ::listen(mHandle, count);
    }
    void ServerSocket::onEvent(RawSocketEvent event) {
        switch (event) {
            case RawSocketEvent::Error: {
                close();
                return;
            }
            case RawSocketEvent::Read: {
                char temp;
                if (recv(mHandle, &temp, sizeof(char), MSG_PEEK) == 0) {
                    close();
                    return;
                }
                if (mVersion == SocketVersion::IPv4) {
                    sockaddr_in address = {};
                    socklen_t len = sizeof(sockaddr_in);
                    SocketHandle clientHandle = ::accept(mHandle, (sockaddr*) &address, &len);
                    if (clientHandle == UNDEFINED_SOCKET) {
                        return;
                    }
                    auto socket = std::make_shared<Socket>(clientHandle, IPEndpoint(&address), mType);
                    if (mAcceptCallback) {
                        mAcceptCallback(socket);
                    }
                } else {
                    sockaddr_in6 address = {};
                    socklen_t len = sizeof(sockaddr_in6);
                    SocketHandle clientHandle = ::accept(mHandle, (sockaddr*) &address, &len);
                    if (clientHandle == UNDEFINED_SOCKET) {
                        return;
                    }
                    auto socket = std::make_shared<Socket>(clientHandle, IPEndpoint(&address), mType);
                    if (mAcceptCallback) {
                        mAcceptCallback(socket);
                    }
                }
                return;
            }
        }
    }
}

#pragma once
#include "IPEndpoint.h"
#include "SocketHandle.h"

namespace Network {
    enum class RawSocketEvent {
        Error, Read, Write
    };
    enum class SocketVersion {
        IPv4, IPv6
    };
    enum class SocketProtocol {
        TCP, UDP
    };
    class RawSocket {
    public:
        RawSocket(SocketProtocol protocol) : mProtocol(protocol) {};
        virtual bool open(SocketVersion version);
        virtual void close();
        inline bool isClosed() const { return mHandle == UNDEFINED_SOCKET; }
        bool bind(IPEndpoint &endpoint) const;
        virtual void update();
    private:
        SocketProtocol mProtocol = SocketProtocol::TCP;
    protected:
        virtual void onEvent(RawSocketEvent event) = 0;
        SocketVersion mVersion = SocketVersion::IPv4;
        SocketHandle mHandle = UNDEFINED_SOCKET;
        PollFD mPollFD{};
    };
}
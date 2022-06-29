#pragma once

namespace Network {
    enum class SocketEvent {
        OnConnected, OnConnectFailed, OnSocketClosed, OnAcceptConnection, OnReceive, OnSend
    };
    enum class TCPError{
         FailedSent, FailedReceive
    };
    enum class UDPError{
        FailedSent, FailedReceive
    };
    enum class ClientError{
        TCPClientClosed, UDPClientClosed
    };
    enum class ServerError{
        FailedAccept, TCPServerClosed, UDPServerClosed

    };

}
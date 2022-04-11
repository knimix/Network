#pragma once

namespace Network {
    enum class Event {
        OnConnect, OnConnectFail, OnDisconnect, OnAcceptConnection, OnReceive, OnSend
    };

}
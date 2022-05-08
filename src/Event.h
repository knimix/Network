#pragma once

namespace Network {
    enum class Event {
        OnConnect, OnConnectFail, OnSocketClose, OnAcceptConnection, OnReceive, OnSend
    };

}
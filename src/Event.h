#pragma once

namespace Network {
    enum class Event {
        None,OnConnect, OnConnectFail, OnDisconnect, OnAcceptConnection, OnReceive, OnSend
    };

}
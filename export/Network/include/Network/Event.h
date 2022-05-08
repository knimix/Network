#pragma once

namespace Network {
    enum class Event {
        None, OnConnect, OnConnectFail, OnError, OnAcceptConnection, OnReceive, OnSend
    };

}
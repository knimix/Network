#pragma once
#include "TCP/ServerSocket.h"
#include "UDP/DatagramSocket.h"


namespace Network{
    bool initialize();
    void terminate();
}
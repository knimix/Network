#pragma once
#include "Socket/TCP/ServerSocket.h"
#include "Socket/UDP/DatagramSocket.h"


namespace Network{
    bool initialize();
    void terminate();
}
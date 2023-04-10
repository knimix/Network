#pragma once
#include "../../src/Socket/TCP/ServerSocket.h"
#include "../../src/Socket/UDP/DatagramSocket.h"


namespace Network{
    bool initialize();
    void terminate();
}
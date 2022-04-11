#pragma once
#include "Socket/TCPSocket/TCPSocket.h"
#include "Socket/UDPSocket/UDPSocket.h"

namespace Network{
    bool Initialize();
    void Terminate();
}
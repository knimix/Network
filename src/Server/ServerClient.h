#pragma once

#include <iostream>
#include "../Packet/PacketStream.h"
#include "../Socket/TCPSocket/TCPSocket.h"
namespace Network{
    class ServerClient{
    public:
        PacketInStream TCPInStream;
        PacketOutStream TCPOutStream;
        PacketOutStream UDPOutStream;
        TCPSocket TCPClient;
        int ClientUDPPort = 0;
        void Kick(){m_ShouldKick = true;}
        inline bool ShouldKick() const{return m_ShouldKick;}
    private:
        bool m_ShouldKick = false;
    };
}
#pragma once
#include "../RawSocket.h"
#include "../../PacketBuffer.h"
#include "../../Packet.h"

namespace Network{
    class Datagram : public Packet{
    public:
        Datagram() = default;
        IPEndpoint Endpoint;
    };

    class DatagramSocket : public RawSocket{
    public:
        DatagramSocket() : RawSocket(SocketProtocol::UDP){};
        PacketBuffer<Datagram> Rx;
        PacketBuffer<Datagram> Tx;
    private:
        void onEvent(RawSocketEvent event) override;
    };
}

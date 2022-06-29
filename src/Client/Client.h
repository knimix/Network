#pragma once
#include "../Packet/PacketStream.h"
#include "../Socket/TCPSocket/TCPSocket.h"
#include "../Socket/UDPSocket/UDPSocket.h"

namespace Network {
    class Client {
    public:
        Client(const char* ip, int tcpPort, int udpPort);
        bool Open();
        void Close();
        void Connect();
        bool Reconnect();
        void Update();
        inline bool IsClosed() const {return m_Closed;}
        PacketInStream TCPInStream;
        PacketOutStream TCPOutStream;
        PacketOutStream UDPOutStream;
        virtual void OnConnected() {}
        virtual void OnConnectionFailed() {}
        virtual void OnConnectionClosed() {}
        virtual void OnClientError(ClientError error) {};
        virtual void OnUDPError(Endpoint& endpoint, UDPError event) {};
        virtual void OnTCPError(TCPError event) {};
        virtual void OnTCPPacketReceived(std::shared_ptr<Packet>& packet) {}
        virtual void OnTCPPacketSent(std::shared_ptr<Packet>& packet) {}
        virtual void OnUDPPacketReceived(std::shared_ptr<Packet>& packet) {}
        virtual void OnUDPPacketSent(std::shared_ptr<Packet>& packet) {}
        virtual void OnUDPInvalidPacketLengthReceived(Endpoint& endpoint, size_t packetSize) {};
        virtual void OnUDPInvalidPacketLengthSend(Endpoint& endpoint, std::shared_ptr<Packet>& packet) {};
        virtual void OnUDPUnknownSourcePacketReceived(Endpoint& endpoint, std::shared_ptr<Packet>& packet) {};
        virtual void OnUDPCorruptPacketReceived(Endpoint& endpoint, size_t& bytesReceived, uint16_t packetSize) {};
        virtual void OnTCPInvalidPacketLengthReceived(size_t packetSize) {};
        virtual void OnTCPInvalidPacketLengthSend(std::shared_ptr<Packet>& packet) {};
        virtual void OnTCPBytesReceived(void* data, size_t size) {};
        virtual void OnTCPBytesSent(void* data, size_t size) {};
        virtual void OnUDPBytesReceived(Endpoint& endpoint, void* data, size_t size) {};
        virtual void OnUDPBytesSent(Endpoint& endpoint, void* data, size_t size) {};
    private:
        void TCPClientEventCallback(SocketEvent event);
        void UDPClientEventCallback(SocketEvent event);
        TCPSocket m_TCPClient;
        UDPSocket m_UDPClient;
        std::string m_IP;
        int m_TCPPort = 0;
        int m_UDPPort = 0;
        std::array<char,MAX_PACKET_SIZE> m_UDPBuffer{};
        Endpoint m_TCPEndpoint;
        Endpoint m_UDPDestination;
        bool m_Closed = true;
    };
}

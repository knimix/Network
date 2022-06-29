#pragma once
#include <unordered_map>
#include <memory>
#include "ServerClient.h"
#include "../Socket/TCPSocket/TCPSocket.h"
#include "../Socket/UDPSocket/UDPSocket.h"

namespace Network {
    class Server {
    public:
        Server() = default;
        bool Open(const char* ip, int tcpPort, int udpPort);
        void Close();
        void Update();
        inline bool IsClosed() const {return m_Closed;}
        inline size_t GetClientCount() const { return m_Clients.size(); }
        virtual void OnClientConnected(std::shared_ptr<ServerClient>& client) {};
        virtual void OnClientDisconnected(std::shared_ptr<ServerClient>& client) {};
        virtual void OnServerError(ServerError error) {};
        virtual void OnUDPError(Endpoint& endpoint, UDPError event) {};
        virtual void OnTCPError(std::shared_ptr<ServerClient>& client, TCPError event) {};
        virtual void OnTCPPacketReceived(std::shared_ptr<ServerClient>& client, std::shared_ptr<Packet>& packet) {}
        virtual void OnTCPPacketSent(std::shared_ptr<ServerClient>& client, std::shared_ptr<Packet>& packet) {}
        virtual void OnUDPPacketReceived(std::shared_ptr<ServerClient>& client, std::shared_ptr<Packet>& packet) {}
        virtual void OnUDPPacketSent(std::shared_ptr<ServerClient>& client, std::shared_ptr<Packet>& packet) {}
        virtual void OnUDPInvalidPacketLengthReceived(Endpoint& endpoint, size_t packetSize) {};
        virtual void OnUDPInvalidPacketLengthSend(Endpoint& endpoint, std::shared_ptr<Packet>& packet) {};
        virtual void OnUDPUnknownSourcePacketReceived(Endpoint& endpoint, std::shared_ptr<Packet>& packet) {};
        virtual void OnUDPCorruptPacketReceived(Endpoint& endpoint, size_t& bytesReceived, uint16_t packetSize) {};
        virtual void OnTCPInvalidPacketLengthReceived(std::shared_ptr<ServerClient>& client, size_t packetSize) {};
        virtual void OnTCPInvalidPacketLengthSend(std::shared_ptr<ServerClient>& client, std::shared_ptr<Packet>& packet) {};
        virtual void OnTCPBytesReceived(std::shared_ptr<ServerClient>& client, void* data, size_t size) {};
        virtual void OnTCPBytesSent(std::shared_ptr<ServerClient>& client, void* data, size_t size) {};
        virtual void OnUDPBytesReceived(Endpoint& endpoint, void* data, size_t size) {};
        virtual void OnUDPBytesSent(Endpoint& endpoint, void* data, size_t size) {};
    private:
        void TCPServerEventCallback(SocketEvent event);
        void UDPServerEventCallback(SocketEvent event);
        void TCPClientEventCallback(std::shared_ptr<ServerClient>& client, SocketEvent event);
        UDPSocket m_UDPServer;
        TCPSocket m_TCPServer;
        std::array<char, MAX_PACKET_SIZE> m_UDPBuffer{};
        std::unordered_multimap<std::string, std::shared_ptr<ServerClient>> m_Clients;
        std::unordered_multimap<std::string, std::shared_ptr<ServerClient>>::iterator m_LastUDP;
        bool m_Closed = true;
    };
}
#include "Client.h"
#include <netinet/in.h>
#include <cstring>
#include <iostream>

namespace Network {
    Client::Client(const char* ip, int tcpPort, int udpPort) : m_IP(ip), m_TCPPort(tcpPort), m_UDPPort(udpPort){}

    bool Client::Open(){
        m_TCPEndpoint = Endpoint(m_IP.c_str(), m_TCPPort);
        m_UDPDestination = Endpoint(m_IP.c_str(), m_UDPPort);
        if (!m_TCPEndpoint || !m_UDPDestination) {
            return false;
        }
        if (!m_TCPClient.Create()) {
            return false;
        }
        if (!m_TCPClient.SetBlocking(false)) {
            return false;
        }
        if (!m_UDPClient.Create()) {
            return false;
        }
        if (!m_UDPClient.SetBlocking(true)) {
            return false;
        }
        size_t sent = 0;
        if (!m_UDPClient.Send(nullptr, 0, sent, m_UDPDestination)) {
            return false;
        }
        auto udpPortPacket = Packet::CreatePacket(0);
        udpPortPacket->AppendUint16(m_UDPClient.GetRemotePort());
        TCPOutStream.Append(udpPortPacket);
        m_TCPClient.SetEventCallback([this](auto&& PH1) { TCPClientEventCallback(std::forward<decltype(PH1)>(PH1)); });
        m_UDPClient.SetEventCallback([this](auto&& PH1) { UDPClientEventCallback(std::forward<decltype(PH1)>(PH1)); });
        m_Closed = false;
        return true;
    }
    void Client::Close() {
        if(m_Closed){
            return;
        }
        m_Closed = true;
        if(!m_TCPClient.IsClosed()){
            m_TCPClient.Close();
        }
        if(!m_UDPClient.IsClosed()){
            m_UDPClient.Close();
        }
        OnConnectionClosed();
    }
    void Client::Connect() {
        if(!m_Closed){
            m_TCPClient.Connect(m_TCPEndpoint);
        }
    }
    bool Client::Reconnect() {
        Close();
        if(!Open()){
            return false;
        }
        Connect();
        return true;
    }
    void Client::TCPClientEventCallback(SocketEvent event) {
        switch (event) {
            case SocketEvent::OnConnected:
                OnConnected();
                break;
            case SocketEvent::OnConnectFailed:
                OnConnectionFailed();
                Close();
                break;
            case SocketEvent::OnSocketClosed:
                OnClientError(ClientError::TCPClientClosed);
                Close();
                break;
            case SocketEvent::OnReceive: {
                size_t bytesReceived = 0;
                bool success;
                if (TCPInStream.CurrentTask == PacketStreamTask::ProcessSize) {
                    success = m_TCPClient.Receive((&TCPInStream.PacketSize) + TCPInStream.CurrentOffset, sizeof(uint16_t) - TCPInStream.CurrentOffset, bytesReceived);
                } else {
                    success = m_TCPClient.Receive((&TCPInStream.Buffer) + TCPInStream.CurrentOffset, TCPInStream.PacketSize - TCPInStream.CurrentOffset, bytesReceived);
                }
                if (!success) {
                    OnTCPError(TCPError::FailedReceive);
                    Close();
                    break;
                }
                if (bytesReceived > 0) {
                    if (TCPInStream.CurrentTask == PacketStreamTask::ProcessSize) {
                        OnTCPBytesReceived((&TCPInStream.PacketSize) + TCPInStream.CurrentOffset, bytesReceived);
                    } else {
                        OnTCPBytesReceived((&TCPInStream.Buffer[0]) + TCPInStream.CurrentOffset, bytesReceived);
                    }
                    TCPInStream.CurrentOffset += bytesReceived;
                    if (TCPInStream.CurrentTask == PacketStreamTask::ProcessSize) {
                        if (TCPInStream.CurrentOffset == sizeof(uint16_t)) {
                            TCPInStream.PacketSize = ntohs(TCPInStream.PacketSize);
                            if (TCPInStream.PacketSize <= 1 || TCPInStream.PacketSize > MAX_PACKET_SIZE) {
                                OnTCPInvalidPacketLengthReceived(TCPInStream.PacketSize);
                                Close();
                                break;
                            }
                            TCPInStream.CurrentOffset = 0;
                            TCPInStream.CurrentTask = PacketStreamTask::ProcessData;
                        }
                    } else {
                        if (TCPInStream.CurrentOffset == TCPInStream.PacketSize) {
                            auto packet = Network::Packet::CreatePacket();
                            packet->GetBuffer().resize(TCPInStream.PacketSize);
                            memcpy(&packet->GetBuffer()[0], &TCPInStream.Buffer[0], TCPInStream.PacketSize);
                            TCPInStream.PacketSize = 0;
                            TCPInStream.CurrentOffset = 0;
                            TCPInStream.CurrentTask = PacketStreamTask::ProcessSize;
                            OnTCPPacketReceived(packet);
                        }
                    }
                }
                break;
            }
            case SocketEvent::OnSend:
                while (!TCPOutStream.Empty()) {
                    size_t bytesSent = 0;
                    bool success;
                    auto& packet = TCPOutStream.Front();
                    auto& buffer = packet->GetBuffer();
                    if (buffer.size() <= 1 || buffer.size() > MAX_PACKET_SIZE) {
                        OnTCPInvalidPacketLengthSend(packet);
                        TCPOutStream.Pop();
                        continue;
                    }
                    uint16_t packetSize = htons(TCPOutStream.Front()->GetBuffer().size());
                    if (TCPOutStream.CurrentTask == PacketStreamTask::ProcessSize) {
                        success = m_TCPClient.Send((&packetSize) + TCPOutStream.CurrentOffset,
                                                   sizeof(uint16_t) - TCPOutStream.CurrentOffset, bytesSent);
                    } else {
                        success = m_TCPClient.Send((&buffer[0]) + TCPOutStream.CurrentOffset,
                                                   buffer.size() - TCPOutStream.CurrentOffset, bytesSent);
                    }
                    if (!success) {
                        OnTCPError(TCPError::FailedSent);
                        Close();
                        break;
                    }
                    if (bytesSent > 0) {
                        if (TCPOutStream.CurrentTask == PacketStreamTask::ProcessSize) {
                            OnTCPBytesSent((&packetSize) + TCPOutStream.CurrentOffset, bytesSent);
                        } else {
                            OnTCPBytesSent((&buffer[0]) + TCPOutStream.CurrentOffset, bytesSent);
                        }
                        TCPOutStream.CurrentOffset += bytesSent;
                        if (TCPOutStream.CurrentTask == PacketStreamTask::ProcessSize) {
                            if (TCPOutStream.CurrentOffset == sizeof(uint16_t)) {
                                TCPOutStream.CurrentOffset = 0;
                                TCPOutStream.CurrentTask = PacketStreamTask::ProcessData;
                            } else {
                                break;
                            }
                        } else {
                            if (TCPOutStream.CurrentOffset == buffer.size()) {
                                TCPOutStream.CurrentOffset = 0;
                                TCPOutStream.CurrentTask = PacketStreamTask::ProcessSize;
                                OnTCPPacketSent(packet);
                                TCPOutStream.Pop();
                            } else {
                                break;
                            }
                        }
                    }
                }
                break;
        }
    }
    void Client::UDPClientEventCallback(SocketEvent event) {
        switch (event) {
            case SocketEvent::OnSocketClosed:
                OnClientError(ClientError::UDPClientClosed);
                Close();
                break;
            case SocketEvent::OnReceive: {
                Endpoint from;
                size_t bytesReceived = 0;
                if (!m_UDPClient.Receive(&m_UDPBuffer[0], MAX_PACKET_SIZE + sizeof(uint16_t), bytesReceived, from)) {
                    OnUDPError(from, UDPError::FailedReceive);
                    break;
                }
                OnUDPBytesReceived(from, &m_UDPBuffer[0], bytesReceived);
                uint16_t packetSize;
                memcpy(&packetSize, &m_UDPBuffer[0], sizeof(uint16_t));
                packetSize = ntohs(packetSize);
                if (packetSize <= 1 || packetSize > MAX_PACKET_SIZE) {
                    OnUDPInvalidPacketLengthReceived(from, packetSize);
                    break;
                }
                if (bytesReceived - packetSize - sizeof(uint16_t) != 0) {
                    OnUDPCorruptPacketReceived(from, bytesReceived, packetSize);
                    break;
                }
                auto packet = Packet::CreatePacket();
                packet->GetBuffer().resize(packetSize);
                memcpy(&packet->GetBuffer()[0], (&m_UDPBuffer[0]) + sizeof(uint16_t), packetSize);
                /*   if (from.GetPort() != m_UDPDestination.GetPort()) {
                       OnUDPUnknownSourcePacketReceived(from, packet);
                       break;
                   }*/
                OnUDPPacketReceived(packet);
                break;
            }
            case SocketEvent::OnSend:
                if (!UDPOutStream.Empty()) {
                    auto packet = UDPOutStream.Front();
                    size_t totalSize = 0;
                    uint16_t packetSize = packet->GetBuffer().size();
                    if (packetSize <= 1 || packetSize > MAX_PACKET_SIZE) {
                        OnUDPInvalidPacketLengthSend(m_UDPDestination, packet);
                        UDPOutStream.Pop();
                        break;
                    }
                    totalSize += packetSize;
                    totalSize += sizeof(uint16_t);
                    packetSize = htons(packetSize);
                    memcpy(&m_UDPBuffer[0], &packetSize, sizeof(uint16_t));
                    memcpy(&m_UDPBuffer[0] + sizeof(uint16_t), &packet->GetBuffer()[0], packet->GetBuffer().size());
                    size_t bytesSent = 0;
                    if (!m_UDPClient.Send(&m_UDPBuffer[0], totalSize, bytesSent, m_UDPDestination)) {
                        OnUDPError(m_UDPDestination, UDPError::FailedSent);
                        UDPOutStream.Pop();
                        break;
                    }
                    OnUDPBytesSent(m_UDPDestination, &m_UDPBuffer[0], bytesSent);
                    if (bytesSent != totalSize) {
                        OnUDPError(m_UDPDestination, UDPError::FailedSent);
                        UDPOutStream.Pop();
                        break;
                    } else {
                        OnUDPPacketSent(packet);
                        UDPOutStream.Pop();
                    }
                }
                break;
        }
    }
    void Client::Update() {
        m_TCPClient.PollEvents();
        m_UDPClient.PollEvents();
    }
}
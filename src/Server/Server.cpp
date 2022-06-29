#include "Server.h"
#include <iostream>
#include <cstring>
#include "../Core.h"

namespace Network {
    bool Server::Open(const char* ip, int tcpPort, int udpPort) {
        Endpoint tcpEndpoint(ip, tcpPort);
        Endpoint udpEndpoint(ip, udpPort);
        if (!tcpEndpoint || !udpEndpoint) {
            return false;
        }
        if (!m_TCPServer.Create()) {
            return false;
        }
        if (!m_TCPServer.SetBlocking(false)) {
            return false;
        }
        if (!m_TCPServer.Bind(tcpEndpoint)) {
            return false;
        }
        if (!m_TCPServer.Listen()) {
            return false;
        }
        if (!m_UDPServer.Create()) {
            return false;
        }
        if (!m_UDPServer.SetBlocking(false)) {
            return false;
        }
        if (!m_UDPServer.Bind(udpEndpoint)) {
            return false;
        }
        m_Closed = false;
        m_TCPServer.SetEventCallback([this](auto&& PH1) { TCPServerEventCallback(std::forward<decltype(PH1)>(PH1)); });
        m_UDPServer.SetEventCallback([this](auto&& PH1) { UDPServerEventCallback(std::forward<decltype(PH1)>(PH1)); });
        return true;
    }
    void Server::Close() {
        if (m_Closed) {
            return;
        }
        m_Closed = true;
        for (auto& client : m_Clients) {
            if (!client.second->TCPClient.IsClosed()) {
                client.second->TCPClient.Close();
            }
        }
        m_Clients.clear();
        if (!m_TCPServer.IsClosed()) {
            m_TCPServer.Close();
        }
        if (m_UDPServer.IsClosed()) {
            m_UDPServer.Close();
        }
    }
    void Server::TCPServerEventCallback(SocketEvent event) {
        switch (event) {
            case SocketEvent::OnSocketClosed:
                OnServerError(ServerError::TCPServerClosed);
                Close();
                break;
            case SocketEvent::OnAcceptConnection: {
                TCPSocket temp;
                if (!m_TCPServer.Accept(temp)) {
                    OnServerError(ServerError::FailedAccept);
                    break;
                }
                auto& client = m_Clients.emplace(temp.GetEndpoint().GetIP(), std::make_shared<ServerClient>())->second;
                client->TCPClient = temp;
                client->TCPClient.SetEventCallback([this, &client](auto&& PH1) {
                    TCPClientEventCallback(client, std::forward<decltype(PH1)>(PH1));
                });
                OnClientConnected(client);
                break;
            }
            default:
                break;
        }
    }
    void Server::UDPServerEventCallback(SocketEvent event) {
        switch (event) {
            case SocketEvent::OnSocketClosed:
                OnServerError(ServerError::UDPServerClosed);
                Close();
                break;
            case SocketEvent::OnReceive: {
                Endpoint from;
                size_t bytesReceived = 0;
                if (!m_UDPServer.Receive(&m_UDPBuffer[0], MAX_PACKET_SIZE + sizeof(uint16_t), bytesReceived, from)) {
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
                if (m_Clients.find(from.GetIP()) == m_Clients.end()) {
                    OnUDPUnknownSourcePacketReceived(from, packet);
                    break;
                }
                auto pair = m_Clients.equal_range(from.GetIP());
                for (auto it = pair.first; it != pair.second; ++it) {
                    OnUDPPacketReceived(it->second, packet);
                }
                break;
            }
            case SocketEvent::OnSend: {
                if (m_Clients.empty()) {
                    break;
                }
                if (m_LastUDP == m_Clients.end()) {
                    m_LastUDP = m_Clients.begin();
                }
                auto& client = m_LastUDP->second;
                m_LastUDP++;
                if (client->ClientUDPPort == 0) {
                    break;
                }
                if (!client->UDPOutStream.Empty()) {
                    auto packet = client->UDPOutStream.Front();
                    size_t totalSize = 0;
                    uint16_t packetSize = packet->GetBuffer().size();
                    Endpoint destination(client->TCPClient.GetEndpoint().GetIP().c_str(), client->ClientUDPPort);
                    if (packetSize <= 1 || packetSize > MAX_PACKET_SIZE) {
                        OnUDPInvalidPacketLengthSend(destination, packet);
                        client->UDPOutStream.Pop();
                        break;
                    }
                    totalSize += packetSize;
                    totalSize += sizeof(uint16_t);
                    packetSize = htons(packetSize);
                    memcpy(&m_UDPBuffer[0], &packetSize, sizeof(uint16_t));
                    memcpy(&m_UDPBuffer[0] + sizeof(uint16_t), &packet->GetBuffer()[0], packet->GetBuffer().size());
                    size_t bytesSent = 0;
                    if (!m_UDPServer.Send(&m_UDPBuffer[0], totalSize, bytesSent, destination)) {
                        OnUDPError(client->TCPClient.GetEndpoint(), UDPError::FailedSent);
                        client->UDPOutStream.Pop();
                        break;
                    }
                    OnUDPBytesSent(destination, &m_UDPBuffer[0], bytesSent);
                    if (bytesSent != totalSize) {
                        OnUDPError(client->TCPClient.GetEndpoint(), UDPError::FailedSent);
                        client->UDPOutStream.Pop();
                    } else {
                        OnUDPPacketSent(client, packet);
                        client->UDPOutStream.Pop();
                    }
                }
                break;
            }
            default:
                break;
        }
    }
    void Server::TCPClientEventCallback(std::shared_ptr<ServerClient>& client, SocketEvent event) {
        auto& clientSocket = client->TCPClient;
        auto& tcpInStream = client->TCPInStream;
        auto& tcpOutStream = client->TCPOutStream;
        switch (event) {
            case SocketEvent::OnSocketClosed:
                client->Kick();
                break;
            case SocketEvent::OnReceive: {
                size_t bytesReceived = 0;
                bool success;
                if (client->TCPInStream.CurrentTask == PacketStreamTask::ProcessSize) {
                    success = clientSocket.Receive((&tcpInStream.PacketSize) + tcpInStream.CurrentOffset, sizeof(uint16_t) - tcpInStream.CurrentOffset, bytesReceived);
                } else {
                    success = clientSocket.Receive((&tcpInStream.Buffer[0]) + tcpInStream.CurrentOffset, tcpInStream.PacketSize - tcpInStream.CurrentOffset, bytesReceived);
                }
                if (!success) {
                    client->Kick();
                    OnTCPError(client, TCPError::FailedReceive);
                    break;
                }
                if (bytesReceived > 0) {
                    if (client->TCPInStream.CurrentTask == PacketStreamTask::ProcessSize) {
                        OnTCPBytesReceived(client, (&tcpInStream.PacketSize) + client->TCPInStream.CurrentOffset, bytesReceived);
                    } else {
                        OnTCPBytesReceived(client, (&tcpInStream.Buffer[0]) + client->TCPInStream.CurrentOffset, bytesReceived);
                    }
                    client->TCPInStream.CurrentOffset += bytesReceived;
                    if (client->TCPInStream.CurrentTask == PacketStreamTask::ProcessSize) {
                        if (tcpInStream.CurrentOffset == sizeof(uint16_t)) {
                            tcpInStream.PacketSize = ntohs(tcpInStream.PacketSize);
                            if (tcpInStream.PacketSize <= 1 || tcpInStream.PacketSize > MAX_PACKET_SIZE) {
                                client->Kick();
                                OnTCPInvalidPacketLengthReceived(client, tcpInStream.PacketSize);
                                break;
                            }
                            tcpInStream.CurrentOffset = 0;
                            tcpInStream.CurrentTask = PacketStreamTask::ProcessData;
                        }
                    } else {
                        if (tcpInStream.CurrentOffset == tcpInStream.PacketSize) {
                            auto packet = Packet::CreatePacket();
                            packet->GetBuffer().resize(tcpInStream.PacketSize);
                            memcpy(&packet->GetBuffer()[0], &tcpInStream.Buffer[0], tcpInStream.PacketSize);
                            tcpInStream.PacketSize = 0;
                            tcpInStream.CurrentOffset = 0;
                            tcpInStream.CurrentTask = PacketStreamTask::ProcessSize;
                            if (client->ClientUDPPort == 0 && packet->GetPacketType() == 0) {
                                client->ClientUDPPort = packet->GetUint16();
                                break;
                            }
                            OnTCPPacketReceived(client, packet);
                            break;
                        }
                    }
                }
                break;
            }
            case SocketEvent::OnSend:
                while (!tcpOutStream.Empty()) {
                    size_t bytesSent = 0;
                    bool success;
                    auto& packet = tcpOutStream.Front();
                    auto& buffer = packet->GetBuffer();
                    if (buffer.size() <= 1 || buffer.size() > MAX_PACKET_SIZE) {
                        tcpOutStream.Pop();
                        OnTCPInvalidPacketLengthSend(client, packet);
                        continue;
                    }
                    uint16_t packetSize = htons(tcpOutStream.Front()->GetBuffer().size());
                    if (tcpOutStream.CurrentTask == PacketStreamTask::ProcessSize) {
                        success = clientSocket.Send((&packetSize) + tcpOutStream.CurrentOffset, sizeof(uint16_t) - tcpOutStream.CurrentOffset, bytesSent);
                    } else {
                        success = clientSocket.Send((&buffer[0]) + tcpOutStream.CurrentOffset, buffer.size() - tcpOutStream.CurrentOffset, bytesSent);
                    }
                    if (!success) {
                        client->Kick();
                        OnTCPError(client, TCPError::FailedSent);
                        break;
                    }
                    if (bytesSent > 0) {
                        if (client->TCPOutStream.CurrentTask == PacketStreamTask::ProcessSize) {
                            OnTCPBytesSent(client, (&packetSize) + tcpOutStream.CurrentOffset, bytesSent);
                        } else {
                            OnTCPBytesSent(client, (&buffer[0]) + tcpOutStream.CurrentOffset, bytesSent);
                        }
                        tcpOutStream.CurrentOffset += bytesSent;
                        if (tcpOutStream.CurrentTask == PacketStreamTask::ProcessSize) {
                            if (tcpOutStream.CurrentOffset == sizeof(uint16_t)) {
                                tcpOutStream.CurrentOffset = 0;
                                tcpOutStream.CurrentTask = PacketStreamTask::ProcessData;
                            } else {
                                break;
                            }
                        } else {
                            if (tcpOutStream.CurrentOffset == buffer.size()) {
                                tcpOutStream.CurrentOffset = 0;
                                tcpOutStream.CurrentTask = PacketStreamTask::ProcessSize;
                                OnTCPPacketSent(client, tcpOutStream.Front());
                                tcpOutStream.Pop();
                            } else {
                                break;
                            }
                        }
                    }
                }
                break;
            default:
                break;
        }
    }
    void Server::Update() {
        m_TCPServer.PollEvents();
        m_UDPServer.PollEvents();
        if (!m_Closed) {
            for (auto it = m_Clients.begin(); it != m_Clients.end();) {
                auto client = it->second;
                client->TCPClient.PollEvents();
                if (client->ShouldKick()) {
                    if (!client->TCPClient.IsClosed()) {
                        client->TCPClient.Close();
                    }
                    OnClientDisconnected(client);
                    it = m_Clients.erase(it);
                } else {
                    ++it;
                }
            }
        }
    }
}
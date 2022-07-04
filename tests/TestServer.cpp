#include <iostream>
#include <future>
#include "../src/Network.h"
#include "../src/Server/Server.h"
#include <thread>


class MyServer : public Network::Server{
public:
    MyServer() = default;


    void OnTCPPacketReceived(std::shared_ptr<Network::ServerClient>& client, std::shared_ptr<Network::Packet>& packet) override{
        std::cout << "TCP: Packet successfuly received! Size: " << packet->PacketSize() << std::endl;
    }
    void OnUDPPacketReceived(std::shared_ptr<Network::ServerClient>& client, std::shared_ptr<Network::Packet>& packet) override{
        std::cout << "UDP: Packet successfuly received! Size: " << packet->PacketSize() << std::endl;
    }
    void OnClientConnected(std::shared_ptr<Network::ServerClient> &client) override {std::cout << "Client connected from: " << client->TCPClient.GetEndpoint().GetIP() << std::endl;

        auto p = Network::Packet::CreatePacket();
        p->AppendUint32(10);

        auto p2 = p;
        auto p3 = p;
        client->TCPOutStream.Append(p);
        client->UDPOutStream.Append(p2);
        client->UDPOutStream.Append(p3);
        client->UDPOutStream.Append(p3);
        client->UDPOutStream.Append(p3);
        client->UDPOutStream.Append(p3);
    }
    void OnClientDisconnected(std::shared_ptr<Network::ServerClient> &client) override{std::cout << "Client disconnected from: " << client->TCPClient.GetEndpoint().GetIP() << std::endl;}
};

int main() {
    Network::Initialize();

    MyServer server;
    std::cout << "Starting Server!\n";
    if (!server.Open("0.0.0.0", 1920, 1920)) {
        std::cout << "Failed to start server!\n";
        exit(0);
    }

    while (!server.IsClosed()) {
        server.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
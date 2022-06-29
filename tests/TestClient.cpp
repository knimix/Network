#include "../src/Network.h"
#include "../src/Client/Client.h"
#include <iostream>
#include <future>
#include <thread>

class MyClient : public Network::Client{
public:
    MyClient(const char* ip, int tcpPort, int udpPort) : Network::Client(ip,tcpPort,udpPort){}
    void OnConnected() override{std::cout << "Connection to server established!\n";

        auto p = Network::Packet::CreatePacket();
        p->AppendUint32(10);

        auto p2 = p;
        auto p3 = p;
        TCPOutStream.Append(p);
        TCPOutStream.Append(p2);
        TCPOutStream.Append(p3);
    }
    void OnConnectionFailed() override{std::cout << "Connection to server failed!\n";}
    void OnConnectionClosed() override{std::cout << "Connection to server closed!\n";}
    void OnTCPPacketReceived(std::shared_ptr<Network::Packet>& packet) override{std::cout << "TCP: Packet from server received!\n";}
    void OnUDPPacketReceived(std::shared_ptr<Network::Packet>& packet) override{std::cout << "UDP: Packet from server received!\n";}
    void OnClientError(Network::ClientError error) override{
        switch (error) {
            case Network::ClientError::UDPClientClosed:{
                std::cout << "UDP Server Closed!\n";
                break;
            }
            case Network::ClientError::TCPClientClosed:{
                std::cout << "TCP Server Closed!\n";
                break;
            }
        }
    }
};

int main() {
    MyClient client("127.0.0.1", 1920, 1920);
    std::cout << "Starting Client!\n";
    if (!client.Open()) {
        std::cout << "Failed to start client!\n";
        exit(0);
    }
    client.Connect();
    while (true) {
        client.Update();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
    }
}
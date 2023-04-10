#include "../include/Network/Network.h"
#include "../src/Socket/TCP/Socket.h"
#include "../src/Socket/TCP/ServerSocket.h"
#include <iostream>
#include <future>
#include <thread>
#include "../src/Socket/UDP/DatagramSocket.h"

int main() {
    Network::initialize();
    std::cout << "TestClient example" << std::endl;
    auto endpoint = Network::IPEndpoint("127.0.0.1", 1920);
    Network::Socket socket(Network::SocketType::Raw);

    std::cout << "Open: " << socket.open(Network::SocketVersion::IPv4) << std::endl;
    socket.setConnectionTimeout(2);
    socket.connect(endpoint,[&socket](bool success){
        if(success){

            auto packet = std::make_shared<Network::Packet>();
            packet->append<uint8_t>('H');
            packet->append<uint8_t>('A');
            packet->append<uint8_t>('L');
            packet->append<uint8_t>('L');
            packet->append<uint8_t>('O');
            socket.Tx.insert(packet);
            std::cout << "Connection established!" << std::endl;
        }else{
            std::cout << "Connection failed, try again in 2 seconds!" << std::endl;
            std::this_thread::sleep_for(std::chrono::seconds(2));
        }
    })    ;

    while (true) {
        socket.update();
        if(socket.Rx.hasNext()){
            auto packet = socket.Rx.next();
            std::cout << "Data received " << std::string(packet->begin(),packet->end()) << std::endl;

        }
        if(socket.isClosed()){
            socket.open(Network::SocketVersion::IPv4);
            socket.reconnect();
        }
    }
}
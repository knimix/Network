#include "../src/Network.h"
#include "../src/Socket/TCP/Socket.h"
#include "../src/Socket/TCP/ServerSocket.h"


#include <iostream>
#include <future>
#include <thread>
#include "../src/Socket/UDP/DatagramSocket.h"




int main() {
    Network::initialize();
    Network::Packet p;

    Network::DatagramSocket s;

    Network::Datagram d;



    p.append<std::string>("Hallo");
    p.append<std::string>("Welt");
    p.append<uint32_t>(123);
    p.append<std::string>("Welt");



    std::cout << p.get<std::string>() << std::endl;
    std::cout << p.get<std::string>() << std::endl;
    std::cout << p.get<uint32_t> () << std::endl;
    std::cout << p.get<std::string>() << std::endl;




       auto endpoint = Network::IPEndpoint("10.122.23.2",1920);

       Network::Socket socket(Network::SocketType::Raw);

       socket.open(Network::SocketVersion::IPv4);
       socket.setConnectionTimeout(2);
       socket.connect(endpoint,[&socket](bool success){
           std::cout << "Connection: " << success << std::endl;
            if(success){
                auto packet = std::make_shared<Network::Packet>();
                std::string t = "123";
                socket.Tx.insert(packet);
                socket.Tx.insert(packet);
                socket.Tx.insert(packet);
                socket.Tx.insert(packet);
                socket.Tx.insert(packet);
            }
       });


       while(true){
           socket.update();
           if(socket.Rx.hasNext()){
               auto packet = socket.Rx.next();
               std::cout << packet->getSize() << std::endl;
               socket.Rx.pop();

           }
           if(socket.isClosed()){
               socket.open(Network::SocketVersion::IPv4);
               socket.reconnect();
           }
       }





}
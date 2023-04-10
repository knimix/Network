#include "../src/Network.h"
#include "../src/Socket/TCP/Socket.h"
#include "../src/Socket/TCP/ServerSocket.h"


#include <iostream>
#include <future>
#include <thread>




int main() {
    Network::initialize();
    Network::Packet p;



    std::string data = "Hallo";

    p.append(data);


    std::cout << p.get<std::string>() << std::endl;


    exit(0);


       auto endpoint = Network::IPEndpoint("127.0.0.1",1920);

       Network::Socket socket;

       socket.open(Network::SocketType::ManagedIPv4);
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
               socket.open(Network::SocketType::ManagedIPv4);
               socket.reconnect();
           }
       }





}
#include <iostream>
#include <future>
#include "../src/Network.h"
#include "../src/Socket/TCP/ServerSocket.h"
#include <thread>



int main() {
    Network::initialize();
       auto endpoint = Network::IPEndpoint("0.0.0.0",1920);
       Network::ServerSocket server(Network::SocketType::Managed);
       std::cout <<  server.open(Network::SocketVersion::IPv4) << std::endl;
       std::cout << server.bind(endpoint) << std::endl;

       server.listen();
       std::shared_ptr< Network::Socket> client;

       server.accept([&client](std::shared_ptr< Network::Socket>& socket){
           std::cout << "Client connected from: " << socket->getEndpoint().getIP()<< std::endl;
           client = socket;
       });
       while(true){
           server.update();
           if(client){
               client->update();

               if(client->Rx.hasNext()){
                   auto packet = client->Rx.next();
                   std::cout << "Read: " << std::string(packet->begin(),packet->end()) << std::endl;
               }
           }
       }
}
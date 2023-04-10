#include <iostream>
#include <future>
#include "../src/Network.h"



int main() {
    Network::initialize();
    std::cout << "TestServer example" << std::endl;

    auto endpoint = Network::IPEndpoint("0.0.0.0", 1920);
    Network::ServerSocket socket(Network::SocketType::Raw);

    std::cout << "Open: " << socket.open(Network::SocketVersion::IPv4) << std::endl;
    std::cout << "Bind: " << socket.bind(endpoint) << std::endl;
    socket.listen();

    std::vector<std::shared_ptr<Network::Socket>> clients;

    socket.accept([&clients](std::shared_ptr<Network::Socket>& socket){
        std::cout << "New connection from : " << socket->getEndpoint().getIP() << std::endl;
        clients.emplace_back(socket);
    });

    while (true) {
        socket.update();
        for(auto it = clients.begin(); it != clients.end();){
            auto client = it.base()->get();
            client->update();
            if(client->Rx.hasNext()){
                auto packet = client->Rx.next();
                std::cout << "Data from " << client->getEndpoint().getIP() << " " << std::string(packet->begin(),packet->end()) << std::endl;
                client->Tx.insert(packet);
            }
            if(client->isClosed()){
                std::cout << "Client disconnected from " << client->getEndpoint().getIP() << std::endl;
                it = clients.erase(it);
            }else{
                it++;
            }
        }

    }
}
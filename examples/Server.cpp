#include <iostream>
#include "Network/Network.h"

class TCPServer : public Network::TCPSocket {
public:
    void init(){
        Create();
        SetBlocking(false);
        Bind(Network::Endpoint("localhost", 25566));
        Listen();
    }
    void run() {
        while (true) {
            Network::Event event;
            if (PollEvents(event)) {
                switch (event) {
                    case Network::Event::OnAcceptConnection: {
                        std::cout << "New Connection\n";
                        sockets.resize(sockets.size() + 1);
                        Accept(sockets.back());
                        break;
                    }
                }
            }
            for(auto socket : sockets){
                if(socket.PollEvents(event)){
                    std::cout << "Event: " << static_cast<int>(event) << "\n";
                    if(event == Network::Event::OnReceive){
                        std::cout << "Received: ";
                        socket.Receive(buffer, 4);
                        std::cout << buffer.data() << "\n";
                        buffer.clear();
                    }
                }
            }
        }
    }
    ~TCPServer(){
        Close();
    }
private:
    std::vector<char> buffer;
    std::vector<Network::TCPSocket> sockets;
};

class UDPServer : public Network::UDPSocket {
public:
    void init(){
        Create();
        SetBlocking(false);
        Bind(Network::Endpoint("localhost", 25566));
    }
    void run() {
        while (true) {
            Network::Event event;
            if (PollEvents(event)) {
                if(event == Network::Event::OnReceive) {
                    std::cout << "Received: ";
                    Network::Endpoint from;
                    Receive(buffer, 4, from);
                    std::cout << buffer.data() << "\n";
                    buffer.clear();
                }
            }
        }
    }
    ~UDPServer(){
        Close();
    }
private:
    std::vector<char> buffer;
};

int main() {
    Network::Initialize();
    //TCPServer server;
    UDPServer server;
    server.init();
    server.run();
    Network::Terminate();
}
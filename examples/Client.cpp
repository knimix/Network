#include <iostream>
#include "Network/Network.h"

class TCPClient : public Network::TCPSocket {
public:
    void init(){
        Create();
        Connect(Network::Endpoint("localhost", 25566));
        buffer = {'T', 'E', 'S', 'T'};
    }
    void run(){
        while(1)
            Send(buffer);
    }
    ~TCPClient(){
        Close();
    }
private:
    std::vector<char> buffer;
};

class UDPClient : public Network::UDPSocket {
public:
    void init(){
        Create();
        buffer = {'T', 'E', 'S', 'T'};
        endpoint = Network::Endpoint("localhost", 25566);
    }
    void run(){
        while(1)
            Send(buffer, endpoint);
    }
    ~UDPClient(){
        Close();
    }
private:
    std::vector<char> buffer;
    Network::Endpoint endpoint;
};

int main() {
    Network::Initialize();
    //TCPClient client;
    UDPClient client;
    client.init();
    client.run();
    Network::Terminate();
}

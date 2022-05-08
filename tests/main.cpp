#include <iostream>
#include <future>
#include "../src/Network.h"

std::future<void> future;

int main(){
    Network::Initialize();


    Network::TCPSocket client;

    client.Create();
    client.SetBlocking(false);
    client.Bind(Network::Endpoint("0.0.0.0",1920));
    client.Listen();

    client.SetEventCallback([](Network::Event event){
       if(event == Network::Event::OnConnect){
           std::cout << "Connected\n";
       }
       if(event == Network::Event::OnError){
           std::cout << "Socket Error\n";
       }
       if(event == Network::Event::OnConnectFail){
           std::cout << "Connection Failed\n";
       }
    });
 //   client.Close();

    //client.Reconnect();


    future = std::async(std::launch::async,[&client](){
        _sleep(6000);
       client.Reconnect();

    });

    while(true){
        client.PollEvents();


    }



    Network::Terminate();
}
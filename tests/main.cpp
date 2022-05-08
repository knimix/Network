#include <iostream>
#include <future>
#include "../src/Network.h"

std::future<void> future;

std::string url = "www.google.com";

//HTTP GET
std::string get_http = "GET / HTTP/1.1\r\nHost: " + url + "\r\nConnection: close\r\n\r\n";

bool sent = false;

int main(){
    Network::Initialize();


    Network::TCPSocket client;

    client.Create();
    client.SetBlocking(false);
    client.Connect(Network::Endpoint("google.com",80));
   // client.Listen();

    client.SetEventCallback([&client](Network::Socket& socket, Network::Event event)
    {
        Network::TCPSocket& tcpSocket = *(Network::TCPSocket*)&socket;

       if(event == Network::Event::OnConnect){
           std::cout << "Connected\n";
       }
       if(event == Network::Event::OnSocketClose){
           std::cout << "Socket Closed!\n";
       }
       if(event == Network::Event::OnConnectFail){
           std::cout << "Connection Failed\n";
       }
       if(event == Network::Event::OnAcceptConnection){
           Network::TCPSocket myClient;
           tcpSocket.Accept(myClient);
           std::cout << "New Connection\n";
       }
       if(event == Network::Event::OnSend){
           if(!sent){
               sent = true;
               std::vector<char> v(get_http.begin(), get_http.end());
               client.Send(v);
           }
       }
       if(event == Network::Event::OnReceive){
           std::vector<char> buffer;
           client.Receive(buffer,1024);
           buffer.emplace_back('\0');
           std::cout << buffer.data();
       }
    });
    //client.Close();

   // client.Reconnect();


    future = std::async(std::launch::async,[&client](){
        _sleep(6000);
        sent = false;
       client.Reconnect();

    });

    while(true){
        client.PollEvents();


    }



    Network::Terminate();
}
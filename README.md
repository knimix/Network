# Network
A cross-platform network library for UDP and TCP 

## Usage

To use the network library on Windows, it must be initialised with ***Network::Initialize();***. This is not necessary on other platforms

## Endpoint

An endpoint is a destination and is used to tell the socket what to bind to. For example, the endpoint could be **Endpoint("0.0.0.0", 123)**, which would mean a local endpoint. It can also be an endpoint to a server or a website: **Endpoint("google.com", 80)**. With **if(Endpoint("example.com"),123)** you can check whether it is a valid Ip or domain.

## Socket Modes

A socket can be operated in two different modes: The first mode is the blocking mode. In blocking mode, all functions, such as **Connect()** or **Accept()**, wait. The event system can still be used but is not absolutely necessary.

The 2nd mode is the NonBlocking mode. In this mode, no function is waited for and everything is evaluated by the event system. This mode is particularly suitable for a server.

You can switch a socket with **SetBlocking(true)** or **SetBlocking(false)**.


## Events
Server:<br />
**OnAcceptConnection**: Called every time a new client connects via TCP.<br /><br />
**OnSocketClose**: Called when the socket is closed (e.g. connection lost or other errors). <br /><br />
Client:<br />
**OnConnect**: Called when the client has connected to a server.<br /><br />
**OnConnectFail**: Called when the connection to the server failed in order to reconnect client.Reconnect();.<br /><br />
**OnSocketClose**: Called when the connection is broken.<br /><br />
**OnReceive**: Is called as soon as the socket has data to receive with socket.Receive(); the data can be read.<br /><br />
**OnSend**: Called every time the socket is ready to send data, this event is almost always called.<br /><br />

## Socket Creation
Each socket must be created as follows:
1. socket **Create();**
2. socket **SetBlocking(true/false);** (default is false)
3. socket **Bind();** (not absolutely necessary for TCP clients)
4. **Connect();** (TCP Client), **Listen();** (TCP Server)

If the socket is running on NonBlocking, a loop must now be created in which socket.PollEvents() is called. The events can be retrieved by ***socket.SetEventCallback();***.
This applies to any socket that is running on nonblocking, including those that a tcp server gets after the Accept(); method.



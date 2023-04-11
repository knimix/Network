# Network
This is the third version of my Network library for C++.
It supports TCP and UDP with IPv4 or IPv6.

### Building
To build this project, you have to clone this repository and type "cmake ." in console while you're in the project directory. Next, you type "make". After building, the  library is in /build folder. All necessary includes are in /include.

### First steps
If you build this library for windows you have to initialize the library with "Network::initialize()" (initializes the winsock api). At the end, you should also do "Network::shutdown()" to terminate the library.

### IPEndpoint
The IPEndpoint is a class that contains an address. This could be a raw address or a domain. It also contains a port where e.g. the client should connect.
An Endpoint can be solved with "solveIPv4" or "solveIPv6". This is used to create internal address data from the domain or address. If you want to just get the IP of a URL, you can use "solveIPv4" and pass a nullptr because you don't need this. After that, you can use "getIP()" to get the IP address. Note that any solve function could fail and it will return false. 




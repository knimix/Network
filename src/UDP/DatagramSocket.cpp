#include <Network/UDP/DatagramSocket.h>
#include <Network/Core.h>

namespace Network {
    void DatagramSocket::onEvent(RawSocketEvent event) {
        switch (event) {
            case RawSocketEvent::Error: {
                close();
                return;
            }
            case RawSocketEvent::Read: {
                if (Rx.hasSpace()) {
                    int bytesReceived;
                    auto datagram = std::make_shared<Datagram>();
                    datagram->resize(MAX_PACKET_SIZE);
                    if (mVersion == SocketVersion::IPv4) {
                        sockaddr_in address;
                        socklen_t length = sizeof(sockaddr_in);
                        bytesReceived = recvfrom(mHandle, (char*) datagram->data(), MAX_PACKET_SIZE, 0, (sockaddr*) &address, &length);
                        datagram->Endpoint = IPEndpoint(&address);
                    } else {
                        sockaddr_in6 address;
                        socklen_t length = sizeof(sockaddr_in6);
                        bytesReceived = recvfrom(mHandle, (char*) datagram->data(), MAX_PACKET_SIZE, 0, (sockaddr*) &address, &length);
                        datagram->Endpoint = IPEndpoint(&address);
                    }
                    if (bytesReceived == -1) {
                        return;
                    }
                    datagram->resize(bytesReceived);
                    Rx.insert(datagram);
                }
                return;
            };
            case RawSocketEvent::Write: {
                if (Tx.hasNext()) {
                    auto datagram = Tx.peek();
                    int sent;
                    if (mVersion == SocketVersion::IPv4) {
                        sockaddr_in address;
                        if (!datagram->Endpoint.solveIPv4(&address)) {
                            return;
                        }
                        sent = sendto(mHandle, (char*) datagram->data(), datagram->getSize(), 0, (sockaddr*) &address, sizeof(sockaddr));
                    } else {
                        sockaddr_in6 address;
                        if (!datagram->Endpoint.solveIPv6(&address)) {
                            return;
                        }
                        sent = sendto(mHandle, (char*) datagram->data(), datagram->getSize(), 0, (sockaddr*) &address, sizeof(sockaddr));
                    }
                    Tx.pop();
                    if (sent == -1) {
                        close();
                    }
                }
                return;
            }
        }
    }
}
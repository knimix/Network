#include "Socket.h"
#include "../../Core.h"

namespace Network {
    Socket::Socket(SocketHandle handle, const IPEndpoint& endpoint, SocketType type) : RawSocket(SocketProtocol::TCP), mEndpoint(endpoint), mType(type) {
        mHandle = handle;
        mPollFD.fd = mHandle;
        mPollFD.events = POLLRDNORM | POLLWRNORM;
        mConnected = true;
        mReceiveBuffer.resize(MAX_PACKET_SIZE + sizeof(uint16_t));
    }
    void Socket::close() {
        if (isClosed()) {
            return;
        }
        RawSocket::close();
        if (mConnecting) {
            mConnecting = false;
            if (!mConnected) {
                if (mConnectionCallback) {
                    mConnectionCallback(false);
                }
            }
        }
        mConnected = false;
    }
    void Socket::connect(IPEndpoint& endpoint, const std::function<void(bool)>& callback) {
        if (mConnected || mConnecting || isClosed()) {
            return;
        }
        mConnectingStart = std::chrono::steady_clock::now();
        mConnectionCallback = callback;
        mEndpoint = endpoint;
        if (mVersion == SocketVersion::IPv4) {
            sockaddr_in address{};
            if (!endpoint.solveIPv4(&address)) {
                callback(false);
                return;
            }
            mConnecting = true;
            ::connect(mHandle, (sockaddr*) &address, sizeof(sockaddr_in));
        } else {
            sockaddr_in6 address{};
            if (!endpoint.solveIPv6(&address)) {
                callback(false);
                return;
            }
            mConnecting = true;
            ::connect(mHandle, (sockaddr*) &address, sizeof(sockaddr_in6));
        }
    }
    bool Socket::reconnect() {
        if (!mConnectionCallback || isClosed()) {
            return false;
        }
        connect(mEndpoint, mConnectionCallback);
        return true;
    }
    void Socket::update() {
        if (mConnecting) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - mConnectingStart).count() >= mConnectionTimeout) {
                close();
            }
        }
        RawSocket::update();
    }
    void Socket::onEvent(RawSocketEvent event) {
        switch (event) {
            case RawSocketEvent::Error: {
                close();
                break;
            }
            case RawSocketEvent::Read: {
                char temp;
                if (recv(mHandle, &temp, sizeof(char), MSG_PEEK) == 0) {
                    close();
                    return;
                }
                if (Rx.hasSpace()) {
                    if (mType == SocketType::Raw) {
                        auto packet = std::make_shared<Packet>();
                        packet->resize(FRAGMENT_SIZE);
                        auto read = ::recv(mHandle, (char*) packet->data(), FRAGMENT_SIZE, 0);
                        if (read == -1) {
                            close();
                            return;
                        }
                        packet->resize(read);
                        Rx.insert(packet);
                    } else {
                        if (mReceiveState == ReceiveState::ProcessSize) {
                            auto read = ::recv(mHandle, (char*) mReceiveBuffer.data() + mReceivedBytes, sizeof(uint16_t) - mReceivedBytes, 0);
                            if (read == -1) {
                                close();
                                return;
                            }
                            mReceivedBytes += read;
                            if (mReceivedBytes == sizeof(uint16_t)) {
                                mReceivedBytes = 0;
                                mReceiveState = ReceiveState::ProcessData;
                                int packetSize = *reinterpret_cast<uint16_t*>(mReceiveBuffer.data());
                            }
                        } else {
                            int packetSize = *reinterpret_cast<uint16_t*>(mReceiveBuffer.data());
                            int left = packetSize - mReceivedBytes;
                            auto read = ::recv(mHandle, (char*) mReceiveBuffer.data() + sizeof(uint16_t) + mReceivedBytes, left, 0);
                            if (read == -1) {
                                close();
                                return;
                            }
                            mReceivedBytes += read;
                            if (mReceivedBytes == packetSize) {
                                mReceiveState = ReceiveState::ProcessSize;
                                auto packet = std::make_shared<Packet>();
                                packet->assign(mReceiveBuffer.begin() + sizeof(uint16_t), mReceiveBuffer.begin() + sizeof(uint16_t) + mReceivedBytes);
                                Rx.insert(packet);
                                mReceivedBytes = 0;
                            }
                        }
                    }
                }
                break;
            }
            case RawSocketEvent::Write: {
                if (!mConnected) {
                    mConnected = true;
                    mConnecting = false;
                    if (mConnectionCallback) {
                        mConnectionCallback(true);
                    }
                }
                if (Tx.hasNext()) {
                    auto& packet = Tx.peek();
                    if (mType == SocketType::Raw) {
                        int sent;
                        int left = packet->getSize() - mSentBytes;
                        if (left > FRAGMENT_SIZE) {
                            sent = ::send(mHandle, (char*) packet->data() + mSentBytes, FRAGMENT_SIZE, 0);
                        } else {
                            sent = ::send(mHandle, (char*) packet->data() + mSentBytes, packet->getSize() - mSentBytes, 0);
                        }
                        if (sent == -1) {
                            close();
                            return;
                        }
                        mSentBytes += sent;
                        if (mSentBytes == packet->getSize()) {
                            mSentBytes = 0;
                            Tx.pop();
                        }
                    } else {
                        if (mSendBuffer.empty()) {
                            uint16_t size = packet->getSize();
                            mSendBuffer.insert(mSendBuffer.end(), (char*) &size, (char*) (&size) + sizeof(uint16_t));
                            mSendBuffer.insert(mSendBuffer.end(), packet->begin(), packet->end());
                        }
                        int left = mSendBuffer.size() - mSentBytes;
                        int sent;
                        if (left > FRAGMENT_SIZE) {
                            sent = ::send(mHandle, (char*) mSendBuffer.data() + mSentBytes, FRAGMENT_SIZE, 0);
                        } else {
                            sent = ::send(mHandle, (char*) mSendBuffer.data() + mSentBytes, mSendBuffer.size() - mSentBytes, 0);
                        }
                        if (sent == -1) {
                            close();
                            return;
                        }
                        mSentBytes += sent;
                        if (mSentBytes == mSendBuffer.size()) {
                            Tx.pop();
                            mSendBuffer.clear();
                            mSentBytes = 0;
                        }
                    }
                }
                break;
            }
        }
    }
}










#include "Socket.h"
#include "../../Core.h"

namespace Network {

    Socket::Socket(SocketHandle handle,const IPEndpoint& endpoint, SocketType type) : mHandle(handle),mEndpoint(endpoint), mType(type){
        mPollFD.fd = mHandle;
        mPollFD.events = POLLRDNORM | POLLWRNORM;
        mConnected = true;
    }

    bool Network::Socket::open(Network::SocketType type) {
        if (!isClosed()) {
            return false;
        }
        mType = type;
        if (mType == SocketType::RawIPv4 || mType == SocketType::ManagedIPv4) {
            mHandle = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
        } else {
            mHandle = socket(AF_INET6, SOCK_STREAM, IPPROTO_TCP);
        }
        if(isClosed()){
            return false;
        }
        mPollFD.fd = mHandle;
        mPollFD.events = POLLRDNORM | POLLWRNORM;
        bool blocking = false;
#ifdef _WIN32
        unsigned long mode = blocking ? 0 : 1;
        return (ioctlsocket(mHandle, FIONBIO, &mode) == 0);
#else
        int flags = fcntl(m_Handle, F_GETFL, 0);
        if (flags == -1) return false;
        flags = blocking ? (flags & ~O_NONBLOCK) : (flags | O_NONBLOCK);
        return (fcntl(mHandle, F_SETFL, flags) == 0);
#endif
    }

    void Network::Socket::close() {
        if(isClosed()){
            return;
        }
        closeSocket(mHandle);
        mHandle = UNDEFINED_SOCKET;
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

    bool Network::Socket::bind(IPEndpoint &endpoint) const {
        if(isClosed()){
            return false;
        }
        if (mType == SocketType::RawIPv4 || mType == SocketType::ManagedIPv4) {
            sockaddr_in address{};
            if (!endpoint.solveIPv4(&address)) {
                return false;
            }
            return ::bind(mHandle, (sockaddr *) &address, sizeof(sockaddr_in)) == 0;
        } else {
            sockaddr_in6 address{};
            if (!endpoint.solveIPv6(&address)) {
                return false;
            }
            return ::bind(mHandle, (sockaddr *) &address, sizeof(sockaddr_in6)) == 0;
        }
    }

    void Network::Socket::connect(Network::IPEndpoint &endpoint, const std::function<void(bool)> &callback) {
        if (mConnected || mConnecting || isClosed()) {
            return;
        }
        mConnectingStart = std::chrono::steady_clock::now();
        mConnectionCallback = callback;
        mEndpoint = endpoint;
        if (mType == SocketType::RawIPv4 || mType == SocketType::ManagedIPv4) {
            sockaddr_in address{};
            if (!endpoint.solveIPv4(&address)) {
                callback(false);
                return;
            }
            mConnecting = true;
            ::connect(mHandle, (sockaddr *) &address, sizeof(sockaddr_in));
        } else {
            sockaddr_in6 address{};
            if (!endpoint.solveIPv6(&address)) {
                callback(false);
                return;
            }
            mConnecting = true;
            ::connect(mHandle, (sockaddr *) &address, sizeof(sockaddr_in6));
        }

    }

    bool Socket::reconnect() {
        if(!mConnectionCallback || isClosed()){
            return false;
        }
        connect(mEndpoint, mConnectionCallback);
        return true;
    }

    void Socket::update() {
        if (isClosed()) {
            return;
        }
        if (mConnecting) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - mConnectingStart).count() >=
                mConnectionTimeout) {
                close();
            }

        }
        pollfd fd = *(pollfd *) &mPollFD;
        if (poll(&fd, 1, 0) > 0) {
            if (fd.revents & POLLERR) {
                close();
                return;
            }
            if (fd.revents & POLLHUP) {
                close();
                return;
            }
            if (fd.revents & POLLNVAL) {
                close();
                return;
            }
            if (fd.revents & POLLRDNORM) {
                char temp;
                if (recv(mHandle, &temp, sizeof(char), MSG_PEEK) == 0) {
                    close();
                    return;
                }
                if(Rx.hasSpace()){
                    if(mType == SocketType::RawIPv4 ||mType == SocketType::RawIPv6){
                        auto packet = std::make_shared<Packet>();
                        packet->getBuffer().resize(FRAGMENT_SIZE);
                        auto read = ::recv(mHandle,(char*)packet->data(), FRAGMENT_SIZE,0);
                        if(read == -1){
                            close();
                            return;
                        }
                        packet->getBuffer().resize(read);
                        Rx.insert(packet);
                    }else{
                        if(mReceiveState == ReceiveState::ProcessSize){
                            auto read = ::recv(mHandle,(char*)mReceiveBuffer.data() + mReceivedBytes,sizeof(uint16_t)-mReceivedBytes,0);
                            if(read == -1){
                                close();
                                return;
                            }
                            mReceivedBytes+= read;
                            if(mReceivedBytes == sizeof(uint16_t)){
                                mReceivedBytes = 0;
                                mReceiveState = ReceiveState::ProcessData;
                                int packetSize = *reinterpret_cast<uint16_t*>(mReceiveBuffer.data());
                            }
                        }else{
                            int packetSize = *reinterpret_cast<uint16_t*>(mReceiveBuffer.data());
                            int left = packetSize - mReceivedBytes;
                            auto read = ::recv(mHandle,(char*)mReceiveBuffer.data() + sizeof(uint16_t) + mReceivedBytes ,left,0);
                            if(read == -1){
                                close();
                                return;
                            }
                            mReceivedBytes+= read;
                            if(mReceivedBytes == packetSize){
                                mReceiveState = ReceiveState::ProcessSize;
                                auto packet = std::make_shared<Packet>();
                                packet->getBuffer().assign(mReceiveBuffer.begin() + sizeof(uint16_t),mReceiveBuffer.begin() + sizeof(uint16_t) + mReceivedBytes);
                                Rx.insert(packet);
                                mReceivedBytes = 0;
                            }
                        }
                    }
                }
                return;
            }
            if (fd.revents & POLLWRNORM) {
                if (!mConnected) {
                    mConnected = true;
                    mConnecting = false;
                    if (mConnectionCallback) {
                        mConnectionCallback(true);
                    }
                }
                if(Tx.hasNext()){
                    auto& packet = Tx.peek();
                    if(mType == SocketType::RawIPv4 ||mType == SocketType::RawIPv6){
                        int sent;
                        int left = packet->size() - mSentBytes;
                        if(left > FRAGMENT_SIZE){
                            sent = ::send(mHandle,(char*)packet->data() + mSentBytes,FRAGMENT_SIZE,0);
                        }else{
                            sent = ::send(mHandle,(char*)packet->data() + mSentBytes,packet->size() - mSentBytes,0);
                        }
                        if(sent == -1){
                            close();
                            return;
                        }
                        mSentBytes+= sent;
                        if(mSentBytes == packet->size()){
                            mSentBytes = 0;
                            Tx.pop();
                        }
                    }else{
                        if(mSendBuffer.empty()){
                            uint16_t size = packet->size();
                            mSendBuffer.insert(mSendBuffer.end(), (char*) &size, (char*) (&size) + sizeof(uint16_t));
                            mSendBuffer.insert(mSendBuffer.end(),packet->begin(),packet->end());
                        }
                        int left = mSendBuffer.size() - mSentBytes;
                        int sent;
                        if(left > FRAGMENT_SIZE){
                            sent = ::send(mHandle,(char*)mSendBuffer.data() + mSentBytes,FRAGMENT_SIZE,0);
                        }else{
                            sent = ::send(mHandle,(char*)mSendBuffer.data() + mSentBytes,mSendBuffer.size() - mSentBytes,0);
                        }
                        if(sent == -1){
                            close();
                            return;
                        }
                        mSentBytes+= sent;
                        if(mSentBytes == mSendBuffer.size()){
                            Tx.pop();
                            mSendBuffer.clear();
                            mSentBytes = 0;
                        }

                    }
                }

                return;
            }
        }
    }




}










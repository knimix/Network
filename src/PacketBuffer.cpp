#include "PacketBuffer.h"

namespace Network{
    bool Network::PacketBuffer::hasNext() {
        std::lock_guard guard(mMutex);
        return !mQueue.empty();
    }
    std::shared_ptr<Network::Packet> Network::PacketBuffer::next() {
        std::lock_guard guard(mMutex);
        auto tmp =  mQueue.front();
        mQueue.pop();
        return tmp;
    }
    std::shared_ptr<Packet> &Network::PacketBuffer::peek() {
        std::lock_guard guard(mMutex);
        return mQueue.front();
    }
    void Network::PacketBuffer::pop() {
        std::lock_guard guard(mMutex);
        mQueue.pop();
    }
    bool Network::PacketBuffer::hasSpace() {
        std::lock_guard guard(mMutex);
        return mQueue.size() < mLimit;
    }

    void Network::PacketBuffer::insert(const std::shared_ptr<Packet> &packet) {
        std::lock_guard guard(mMutex);
        mQueue.emplace(packet);
    }
}



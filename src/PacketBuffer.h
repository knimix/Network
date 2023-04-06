#pragma once
#include <queue>
#include <mutex>
#include <memory>
#include "Packet.h"
namespace Network{
    class PacketBuffer {
    public:
        PacketBuffer() = default;
        void setLimit(int limit){mLimit = limit;}
        bool hasNext();
        std::shared_ptr<Packet> next();
        std::shared_ptr<Packet>& peek();
        void pop();
        bool hasSpace();
        void insert(const std::shared_ptr<Packet>& packet);
    private:
        int mLimit = 1;
        std::queue<std::shared_ptr<Packet>> mQueue;
        mutable std::mutex mMutex;
    };
}

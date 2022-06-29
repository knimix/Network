#pragma once
#include <queue>
#include <memory>
#include <mutex>
#include <array>
#include "Packet.h"

namespace Network {
    enum class PacketStreamTask {
        ProcessSize, ProcessData
    };
    class PacketStream {
    public:
        PacketStream() = default;
        uint64_t CurrentOffset = 0;
        PacketStreamTask CurrentTask = PacketStreamTask::ProcessSize;
    };
    class PacketOutStream : public PacketStream {
    public:
        PacketOutStream() = default;
        void Append(std::shared_ptr<Packet>& packet);
        std::shared_ptr<Packet>& Front();
        void Pop();
        inline bool Empty() { return m_Packets.empty(); }
    private:
        std::queue<std::shared_ptr<Packet>> m_Packets;
        mutable std::mutex m_Mutex;
    };
    class PacketInStream : public PacketStream {
    public:
        PacketInStream() = default;
        std::array<char,MAX_PACKET_SIZE> Buffer{};
        uint16_t PacketSize = 0;
    };
}


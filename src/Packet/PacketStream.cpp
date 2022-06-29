#include "PacketStream.h"

namespace Network {
    void PacketOutStream::Append(std::shared_ptr<Packet>& packet) {
        std::lock_guard lock(m_Mutex);
        m_Packets.push(std::move(packet));
    }
    std::shared_ptr<Packet>& PacketOutStream::Front() {
        std::lock_guard lock(m_Mutex);
        return m_Packets.front();
    }
    void PacketOutStream::Pop() {
        std::lock_guard lock(m_Mutex);
        m_Packets.pop();
    }
}
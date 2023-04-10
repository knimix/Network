#include "Packet.h"

namespace Network {
    void Packet::resize(size_t size) {
        if (size > MAX_PACKET_SIZE) {
            throw PacketException(PacketExceptionType::ResizeError);
        }
        mBuffer.resize(size);
    }
    void Packet::assign(std::vector<uint8_t>::iterator first, std::vector<uint8_t>::iterator last) {
        if (std::distance(first, last) > MAX_PACKET_SIZE) {
            throw PacketException(PacketExceptionType::AssignError);
        }
        mBuffer.assign(first, last);
    }
    void Packet::appendRaw(char* data, int size) {
        if (mBuffer.size() + size > MAX_PACKET_SIZE) {
            throw PacketException(PacketExceptionType::AppendError);
        }
        mBuffer.insert(mBuffer.end(), (char*) data, (char*) (data + size));
    }
    void Packet::erase(int size) {
        mBuffer.erase(mBuffer.begin(), mBuffer.begin() + size);
    }
}
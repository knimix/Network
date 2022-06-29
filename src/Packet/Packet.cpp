#include "Packet.h"
#include "../Core.h"

namespace Network {
    Packet::Packet(PacketType type) :  m_PacketType(type){
        m_Buffer.reserve(MAX_PACKET_SIZE);
        Clear();
    }
    void Packet::Clear() {
        m_Buffer.resize(sizeof(PacketType));
        m_ReadOffset = sizeof(PacketType);
        SetPacketType(m_PacketType);
    }
    PacketType Packet::GetPacketType(){
        auto packetTypePtr = reinterpret_cast<PacketType*>(&m_Buffer[0]);
        return ntohs(*packetTypePtr);
    }
    void Packet::SetPacketType(PacketType type) {
        auto packetTypePtr = reinterpret_cast<PacketType*>(&m_Buffer[0]);
        *packetTypePtr = htons(type);
    }
    void Packet::AppendUint8(uint8_t data) {
        Append(&data, sizeof(uint8_t));
    }
    uint8_t Packet::GetUint8() {
        uint8_t data = *reinterpret_cast<uint8_t*>(&m_Buffer[m_ReadOffset]);
        m_ReadOffset += sizeof(uint8_t);
        return data;
    }
    void Packet::AppendUint16(uint16_t data) {
        data = htons(data);
        Append(&data, sizeof(uint16_t));
    }
    uint16_t Packet::GetUint16() {
        uint16_t data = *reinterpret_cast<uint16_t*>(&m_Buffer[m_ReadOffset]);
        m_ReadOffset += sizeof(uint16_t);
        return ntohs(data);
    }
    void Packet::AppendUint32(uint32_t data) {
        data = htonl(data);
        Append(&data, sizeof(uint32_t));
    }
    uint32_t Packet::GetUint32() {
        uint32_t data = *reinterpret_cast<uint32_t*>(&m_Buffer[m_ReadOffset]);
        m_ReadOffset += sizeof(uint32_t);
        return ntohl(data);
    }
    std::shared_ptr<Packet> Packet::CreatePacket(uint16_t id) {
        return std::make_shared<Packet>(id);
    }
    void Packet::Append(const void* data, uint32_t size) {
        if (m_Buffer.size() + size > MAX_PACKET_SIZE) {
            return;
        }
        m_Buffer.insert(m_Buffer.end(), (char*) data, (char*) data + size);
    }


}



#include "Packet.h"
#include "../Core.h"

namespace Network {
    Packet::Packet(PacketType type) : m_PacketType(type) {
        m_Buffer.reserve(MAX_PACKET_SIZE);
        Clear();
    }
    void Packet::Clear() {
        m_Buffer.resize(sizeof(PacketType));
        m_ReadOffset = sizeof(PacketType);
        SetPacketType(m_PacketType);
    }
    PacketType Packet::GetPacketType() {
        auto packetTypePtr = reinterpret_cast<PacketType*>(&m_Buffer[0]);
        return ntohs(*packetTypePtr);
    }
    void Packet::SetPacketType(PacketType type) {
        auto packetTypePtr = reinterpret_cast<PacketType*>(&m_Buffer[0]);
        *packetTypePtr = htons(type);
    }
    std::shared_ptr<Packet> Packet::CreatePacket(uint16_t id) {
        return std::make_shared<Packet>(id);
    }
    void Packet::AppendUint8(uint8_t data) {
        Append(&data, sizeof(uint8_t));
    }
    void Packet::AppendInt8(int8_t data) {
        Append(&data, sizeof(int8_t));
    }
    void Packet::AppendUint16(uint16_t data) {
        data = htons(data);
        Append(&data, sizeof(uint16_t));
    }
    void Packet::AppendInt16(int16_t data) {
        AppendUint16(static_cast<uint16_t>(data));
    }
    void Packet::AppendUint32(uint32_t data) {
        data = htonl(data);
        Append(&data, sizeof(uint32_t));
    }
    void Packet::AppendInt32(int32_t data) {
        AppendUint32(static_cast<uint32_t>(data));
    }
    void Packet::AppendUint64(uint64_t data) {
        Append(&data, sizeof(uint64_t));
    }
    void Packet::AppendInt64(int64_t data) {
        Append(&data, sizeof(int64_t));
    }
    void Packet::AppendBool(bool data) {
        Append(&data, sizeof(bool));
    }
    void Packet::AppendFloat(float data) {
        Append(&data, sizeof(float));
    }
    void Packet::AppendDouble(double data) {
        Append(&data, sizeof(double));
    }
    void Packet::AppendString(const std::string& data) {
        AppendUint32(data.length());
        Append(data.data(), data.size());
    }
    uint8_t Packet::GetUint8() {
        return *reinterpret_cast<uint8_t*>(Get(sizeof(uint8_t)));
    }
    int8_t Packet::GetInt8() {
        return *reinterpret_cast<int8_t*>(Get(sizeof(int8_t)));
    }
    uint16_t Packet::GetUint16() {
        return ntohs(*reinterpret_cast<uint16_t*>(Get(sizeof(uint16_t))));
    }
    int16_t Packet::GetInt16() {
        return static_cast<int16_t>(GetUint16());
    }
    uint32_t Packet::GetUint32() {
        uint32_t data = *reinterpret_cast<uint32_t*>(Get(sizeof(uint32_t)));
        return ntohl(data);
    }
    int32_t Packet::GetInt32() {
        return static_cast<int32_t>(GetUint32());
    }
    uint64_t Packet::GetUint64() {
        return *reinterpret_cast<uint64_t*>(Get(sizeof(uint64_t)));
    }
    int64_t Packet::GetInt64() {
        return *reinterpret_cast<int64_t*>(Get(sizeof(int64_t)));
    }
    bool Packet::GetBool() {
        return *reinterpret_cast<bool*>(Get(sizeof(bool)));;
    }
    float Packet::GetFloat() {
        return *reinterpret_cast<float*>(Get(sizeof(float)));;
    }
    double Packet::GetDouble() {
        return *reinterpret_cast<double*>(Get(sizeof(double)));;
    }
    std::string Packet::GetString() {
        auto size = GetUint32();
        auto data = Get(size);
        return {(char*) data, (char*) data + size};
    }
    void Packet::Append(const void* data, uint32_t size) {
        if (m_Buffer.size() + size > MAX_PACKET_SIZE) {
            throw PacketException("Failed to append data to packet (packet is full)");
        }
        m_Buffer.insert(m_Buffer.end(), (char*) data, (char*) data + size);
    }
    void* Packet::Get(uint32_t size) {
        if (m_ReadOffset + size > m_Buffer.size()) {
            throw PacketException("Failed to get data from packet (no more data)");
        }
        auto pointer = &m_Buffer[m_ReadOffset];
        m_ReadOffset += size;
        return pointer;
    }
}



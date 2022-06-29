#pragma once
#include <vector>
#include <cstdint>
#include <memory>

#define MAX_PACKET_SIZE 1024 + 2
typedef uint16_t PacketType;
namespace Network {
    class Packet {
    public:
        explicit Packet(PacketType type = 1);
        void Clear();
        PacketType GetPacketType();
        void SetPacketType(PacketType id);
        void AppendUint16(uint16_t data);
        uint16_t GetUint16();
        void AppendUint32(uint32_t data);
        uint32_t GetUint32();
        inline std::vector<char>& GetBuffer() { return m_Buffer; }
        inline size_t PacketSize() const { return m_Buffer.size(); }
        static std::shared_ptr<Packet> CreatePacket(uint16_t packetID = 1);
    private:
        void Append(const void* data, uint32_t size);
        std::vector<char> m_Buffer;
        uint32_t m_ReadOffset = 0;
        PacketType m_PacketType = 1;
    };
}
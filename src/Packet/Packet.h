#pragma once
#include <utility>
#include <vector>
#include <cstdint>
#include <memory>
#include <string>

#define MAX_PACKET_SIZE 2048 + 2
typedef uint16_t PacketType;
namespace Network {
    class PacketException : public std::exception {
    public:
        explicit PacketException(std::string msg) : m_Msg(std::move(msg)) {}
        const char* what() const noexcept override { return m_Msg.c_str(); }
    private:
        std::string m_Msg;
    };
    class Packet {
    public:
        explicit Packet(PacketType type = 1);
        void Clear();
        PacketType GetPacketType();
        void SetPacketType(PacketType id);
        void AppendUint8(uint8_t data);
        void AppendInt8(int8_t data);
        void AppendUint16(uint16_t data);
        void AppendInt16(int16_t data);
        void AppendUint32(uint32_t data);
        void AppendInt32(int32_t data);
        void AppendUint64(uint64_t data);
        void AppendInt64(int64_t data);
        void AppendBool(bool data);
        void AppendFloat(float data);
        void AppendDouble(double data);
        void AppendString(const std::string& data);
        uint8_t GetUint8();
        int8_t GetInt8();
        uint16_t GetUint16();
        int16_t GetInt16();
        uint32_t GetUint32();
        int32_t GetInt32();
        uint64_t GetUint64();
        int64_t GetInt64();
        bool GetBool();
        float GetFloat();
        double GetDouble();
        std::string GetString();
        void Append(const void* data, uint32_t size);
        void* Get(uint32_t size);
        inline std::vector<uint8_t>& GetBuffer() { return m_Buffer; }
        inline size_t PacketSize() const { return m_Buffer.size(); }
        static std::shared_ptr<Packet> CreatePacket(uint16_t packetID = 1);
    private:
        std::vector<uint8_t> m_Buffer;
        uint32_t m_ReadOffset = sizeof(PacketType);
        PacketType m_PacketType = 1;
    };
}
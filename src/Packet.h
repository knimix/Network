#pragma once
#include <vector>
#include <string>
#include <cstdint>
#include <string_view>
#include <optional>
#include <exception>
#include <stdexcept>
#include <array>

#define MAX_PACKET_SIZE 65536
namespace Network {
    enum class PacketExceptionType {
        AppendError, GetError, ResizeError, AssignError
    };
    class PacketException {
    public:
        PacketException(PacketExceptionType type) : mType(type) {}
        PacketExceptionType getType() { return mType; }
    private:
        PacketExceptionType mType;
    };
    class Packet {
    public:
        Packet() = default;
        void resize(size_t size);
        void assign(std::vector<uint8_t>::iterator first, std::vector<uint8_t>::iterator last);
        inline std::vector<uint8_t>::iterator begin() { return mBuffer.begin(); }
        inline std::vector<uint8_t>::iterator end() { return mBuffer.end(); }
        inline size_t getSize() { return mBuffer.size(); }
        inline uint8_t* data() { return mBuffer.data(); }
        template<typename T>
        typename std::enable_if<!std::is_class<T>::value, void>::type append(T t) {
            appendRaw((char*)&t, sizeof(T));
        }
        template<typename T>
        typename std::enable_if<std::is_same<T, std::string>::value || std::is_same<T, std::string_view>::value, void>::type append(T t) {
            uint16_t size = t.size();
            appendRaw((char*) &size, sizeof(uint16_t));
            appendRaw((char*) t.data(), size);
        }
        template<typename T>
        typename std::enable_if<!std::is_class<T>::value, T>::type get() {
            auto size = sizeof(T);
            if (mBuffer.size() < size) {
                throw PacketException(PacketExceptionType::GetError);
            }
            auto data = *reinterpret_cast<T*>(mBuffer.data());
            erase(size);
            return data;
        }
        template<typename T>
        typename std::enable_if<std::is_same<T, std::string>::value, T>::type get() {
            uint16_t size = get<uint16_t>();
            if (mBuffer.size() < size) {
                throw PacketException(PacketExceptionType::GetError);
            }
            auto data = std::string(mBuffer.begin(), mBuffer.begin() + size);
            erase(size);
            return data;
        }
    private:
        void appendRaw(char* data, int size);
        void erase(int size);
        std::vector<uint8_t> mBuffer;
    };
}

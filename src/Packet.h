#pragma once
#include <vector>
#include <cstdint>

#define MAX_PACKET_SIZE 65536

namespace Network{
    class Packet {
    public:
        Packet() = default;
        uint8_t* data(){return mBuffer.data();}
        std::vector<uint8_t>::iterator begin(){return mBuffer.begin();}
        std::vector<uint8_t>::iterator end(){return mBuffer.end();}
        size_t size(){return mBuffer.size();}
       std::vector<uint8_t>& getBuffer(){return mBuffer;}
    private:
        std::vector<uint8_t> mBuffer;
    };
}

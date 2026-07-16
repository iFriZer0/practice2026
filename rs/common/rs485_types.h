#ifndef RS485_TYPES_H__
#define RS485_TYPES_H__

#include <cstdint>
#include <string>
#include <vector>

struct SendDataResult
{
    bool success{false};
    uint32_t channel_id{0};
    std::string error_message;
};

struct ReceiveDataPacket
{
    std::vector<uint8_t> bytes;
};

struct ReceiveDataResult
{
    bool success{false};
    uint32_t channel_id{0};
    std::vector<ReceiveDataPacket> packets;
    std::string error_message;
};

#endif
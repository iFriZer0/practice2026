#ifndef RS485_TYPES_H__
#define RS485_TYPES_H__

#include <cstdint>
#include <string>
#include <vector>

struct SendDataResult
{
    bool success;
    uint32_t channel_id;
    std::string error_message;
};

struct ReceiveDataResult
{
    bool success;
    uint32_t channel_id;
    std::vector<uint8_t> data;
    std::string error_message;
};

#endif
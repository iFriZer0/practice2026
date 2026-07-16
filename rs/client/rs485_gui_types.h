#ifndef RS485_GUI_TYPES_H__
#define RS485_GUI_TYPES_H__

#include <cstdint>
#include <string>
#include <vector>

struct Rs485SendResult
{
    bool success{false};
    uint32_t channel_id{0};
    std::string error_message;
};

struct Rs485ReceiveResult
{
    bool success{false};
    uint32_t channel_id{0};
    std::vector<uint8_t> data;
    std::string error_message;
};

#endif
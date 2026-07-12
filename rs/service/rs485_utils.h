#ifndef RS485_UTILS_H__
#define RS485_UTILS_H__

#include <cstdint>
#include <string>
#include <vector>

std::vector<uint8_t> parseBytes(
    const std::string &text
);

std::string bytesToHex(
    const std::vector<uint8_t> &bytes
);

#endif
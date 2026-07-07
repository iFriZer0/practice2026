#ifndef RS485_UTILS_H__
#define RS485_UTILS_H__

#include <string>
#include <vector>
#include <cstdint>

std::vector<uint8_t> parseBytes(const std::string &text);

std::string bytesToHex(const std::string &bytes);

#endif
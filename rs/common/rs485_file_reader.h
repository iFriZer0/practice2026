#ifndef RS485_FILE_READER_H
#define RS485_FILE_READER_H

#include <cstdint>
#include <string>
#include <vector>

enum class Rs485FileFormat
{
    Auto,
    Text,
    Binary
};

class Rs485FileReader
{
public:
    static std::vector<uint8_t> read(
        const std::string &file_path,
        Rs485FileFormat format = Rs485FileFormat::Auto
    );

private:
    static Rs485FileFormat determineFormat(
        const std::string &file_path
    );

    static std::vector<uint8_t> readTextFile(
        const std::string &file_path
    );

    static std::vector<uint8_t> readBinaryFile(
        const std::string &file_path
    );
};

#endif
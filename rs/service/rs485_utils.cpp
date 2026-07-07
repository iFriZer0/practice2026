#include <vector>
#include <string>
#include <sstream>
#include <iomanip>
#include <cstdint>
#include <stdexcept>

std::vector<uint8_t> parseBytes(const std::string &text)
{
    std::vector<uint8_t> result;
    std::stringstream stream(text);
    std::string byte_string;

    while (stream >> byte_string)
    {
        unsigned long value = std::stoul(byte_string, nullptr, 16);

        if (value > 255)
        {
            throw std::invalid_argument("Значение больше одного байта: " + byte_string);
        }

        result.push_back(static_cast<uint8_t>(value));
    }

    return result;
}

std::string bytesToHex(const std::string &bytes)
{
    std::ostringstream stream;

    stream << std::uppercase << std::hex << std::setfill('0');

    for (size_t i = 0; i < bytes.size(); ++i)
    {
        unsigned int value =
            static_cast<unsigned char>(bytes[i]);

        stream << std::setw(2) << value;

        if (i + 1 != bytes.size())
        {
            stream << " ";
        }
    }

    return stream.str();
}
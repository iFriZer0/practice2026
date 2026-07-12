#include "rs485_utils.h"

#include "rs485_errors.h"

#include <cstddef>
#include <cstdint>
#include <iomanip>
#include <sstream>
#include <string>
#include <vector>

std::vector<uint8_t> parseBytes(
    const std::string &text)
{
    if (text.empty())
    {
        throw Rs485ValidationException(
            "No data was provided for sending"
        );
    }

    std::vector<uint8_t> result;
    std::stringstream stream(text);
    std::string byte_string;

    while (stream >> byte_string)
    {
        std::size_t parsed_count{0};
        unsigned long value{0};

        try
        {
            value = std::stoul(
                byte_string,
                &parsed_count,
                16
            );
        }
        catch (const std::invalid_argument &)
        {
            throw Rs485ValidationException(
                "Invalid hexadecimal byte: " +
                byte_string
            );
        }
        catch (const std::out_of_range &)
        {
            throw Rs485ValidationException(
                "Hexadecimal value is out of range: " +
                byte_string
            );
        }

        if (parsed_count != byte_string.size())
        {
            throw Rs485ValidationException(
                "Invalid hexadecimal byte: " +
                byte_string
            );
        }

        if (byte_string.size() > 2 || value > 255UL)
        {
            throw Rs485ValidationException(
                "Value does not fit into one byte: " +
                byte_string
            );
        }

        result.push_back(
            static_cast<uint8_t>(value)
        );
    }

    if (result.empty())
    {
        throw Rs485ValidationException(
            "No data was provided for sending"
        );
    }

    return result;
}

std::string bytesToHex(
    const std::vector<uint8_t> &bytes)
{
    std::ostringstream stream;

    stream
        << std::uppercase
        << std::hex
        << std::setfill('0');

    for (std::size_t index = 0;
         index < bytes.size();
         ++index)
    {
        stream
            << std::setw(2)
            << static_cast<unsigned int>(
                bytes[index]
            );

        if (index + 1 < bytes.size())
        {
            stream << ' ';
        }
    }

    return stream.str();
}
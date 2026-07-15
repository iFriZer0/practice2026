#include "rs485_file_reader.h"

#include "rs485_errors.h"
#include "rs485_utils.h"

#include <algorithm>
#include <cctype>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <vector>

std::vector<uint8_t> Rs485FileReader::read(
    const std::string &file_path,
    Rs485FileFormat format)
{
    if (file_path.empty())
    {
        throw Rs485ValidationException(
            "File path is empty"
        );
    }

    const Rs485FileFormat actual_format =
        format == Rs485FileFormat::Auto
            ? determineFormat(file_path)
            : format;

    if (actual_format == Rs485FileFormat::Text)
    {
        return readTextFile(file_path);
    }

    return readBinaryFile(file_path);
}

Rs485FileFormat Rs485FileReader::determineFormat(
    const std::string &file_path)
{
    const std::size_t dot_position =
        file_path.find_last_of('.');

    if (dot_position == std::string::npos)
    {
        throw Rs485ValidationException(
            "File has no extension"
        );
    }

    std::string extension =
        file_path.substr(dot_position);

    std::transform(
        extension.begin(),
        extension.end(),
        extension.begin(),
        [](unsigned char symbol)
        {
            return static_cast<char>(
                std::tolower(symbol)
            );
        }
    );

    if (extension == ".txt" ||
        extension == ".hex" ||
        extension == ".csv")
    {
        return Rs485FileFormat::Text;
    }

    if (extension == ".bin" ||
        extension == ".dat")
    {
        return Rs485FileFormat::Binary;
    }

    throw Rs485ValidationException(
        "Unsupported file extension: " +
        extension
    );
}

std::vector<uint8_t> Rs485FileReader::readTextFile(
    const std::string &file_path)
{
    std::ifstream file(file_path);

    if (!file.is_open())
    {
        throw Rs485ValidationException(
            "Failed to open file: " +
            file_path
        );
    }

    std::ostringstream buffer;
    buffer << file.rdbuf();

    if (file.bad())
    {
        throw Rs485ValidationException(
            "Failed to read file: " +
            file_path
        );
    }

    std::string content = buffer.str();

    if (content.empty())
    {
        throw Rs485ValidationException(
            "Selected file is empty"
        );
    }

    std::replace(
        content.begin(),
        content.end(),
        ',',
        ' '
    );

    std::replace(
        content.begin(),
        content.end(),
        ';',
        ' '
    );

    std::replace(
        content.begin(),
        content.end(),
        '\n',
        ' '
    );

    std::replace(
        content.begin(),
        content.end(),
        '\r',
        ' '
    );

    std::replace(
        content.begin(),
        content.end(),
        '\t',
        ' '
    );

    return parseBytes(content);
}

std::vector<uint8_t> Rs485FileReader::readBinaryFile(
    const std::string &file_path)
{
    std::ifstream file(
        file_path,
        std::ios::binary
    );

    if (!file.is_open())
    {
        throw Rs485ValidationException(
            "Failed to open binary file: " +
            file_path
        );
    }

    const std::vector<char> raw_data{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };

    if (file.bad())
    {
        throw Rs485ValidationException(
            "Failed to read binary file: " +
            file_path
        );
    }

    if (raw_data.empty())
    {
        throw Rs485ValidationException(
            "Selected file is empty"
        );
    }

    std::vector<uint8_t> bytes;
    bytes.reserve(raw_data.size());

    for (const char value : raw_data)
    {
        bytes.push_back(
            static_cast<uint8_t>(
                static_cast<unsigned char>(value)
            )
        );
    }

    return bytes;
}
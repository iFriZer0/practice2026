#ifndef RS485_ERRORS_H__
#define RS485_ERRORS_H__

#include <string>

enum class Rs485Error
{
    None,

    InvalidHex,

    ConnectionError,

    NoReply,

    Timeout,

    Unknown
};

std::string errorToString(
    Rs485Error error);

#endif
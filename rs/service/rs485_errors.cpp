#include "rs485_errors.h"

std::string errorToString(
    Rs485Error error)
{
    switch (error)
    {
        case Rs485Error::None:
            return "No error";

        case Rs485Error::InvalidHex:
            return "Invalid HEX string";

        case Rs485Error::ConnectionError:
            return "Connection error";

        case Rs485Error::NoReply:
            return "No reply";

        case Rs485Error::Timeout:
            return "Timeout";

        default:
            return "Unknown error";
    }
}
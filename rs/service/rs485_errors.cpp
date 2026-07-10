#include "rs485_errors.h"

std::string clientErrorToString(ClientError error)
{
    switch (error)
    {
        case ClientError::None:
            return "No client error";

        case ClientError::NotConnected:
            return "The RS-485 service is not connected";

        case ClientError::InvalidEndpoint:
            return "The RS-485 driver endpoint is invalid";

        case ClientError::InvalidHex:
            return "The byte sequence contains invalid hexadecimal data";

        case ClientError::EmptyData:
            return "No data was provided for sending";

        case ClientError::EmptyCallback:
            return "No receive callback was provided";

        case ClientError::StreamCreationFailed:
            return "Failed to create the gRPC stream";

        case ClientError::StreamWriteFailed:
            return "Failed to write the request to the gRPC stream";

        case ClientError::NoResponse:
            return "The RS-485 driver did not return a response";

        case ClientError::Unknown:
        default:
            return "Unknown client error";
    }
}

std::string pultErrorToString(
    rs485::driver::v1::PultErrors error)
{
    using namespace rs485::driver::v1;

    switch (error)
    {
        case NO_ERROR:
            return "No driver error";

        case WRONG_BRIDGE_ID:
            return "Invalid bridge identifier";

        case NO_REPLY:
            return "No reply from the transport layer";

        case WRONG_INTERFACE:
            return "Invalid interface or channel";

        case TIMEOUT:
            return "The operation timed out";

        case EMPTY_BUFFER:
            return "The received buffer is empty or incomplete";

        case WRONG_PARAM:
            return "Invalid parameters were provided";

        default:
            return "Unknown RS-485 driver error";
    }
}

std::string grpcStatusToString(const grpc::Status &status)
{
    if (status.ok())
    {
        return "No gRPC error";
    }

    std::string result =
        "gRPC error " +
        std::to_string(static_cast<int>(status.error_code()));

    if (!status.error_message().empty())
    {
        result += ": ";
        result += status.error_message();
    }

    return result;
}
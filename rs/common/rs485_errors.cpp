#include "rs485_errors.h"

#include <string>

namespace
{

std::string driverErrorMessage(
    rs485::driver::v1::PultErrors error)
{
    using namespace rs485::driver::v1;

    switch (error)
    {
        case NO_ERROR:
            return "No RS-485 driver error";

        case WRONG_BRIDGE_ID:
            return "Invalid bridge identifier";

        case NO_REPLY:
            return "No reply from the transport layer";

        case WRONG_INTERFACE:
            return "Invalid interface or channel";

        case TIMEOUT:
            return "The RS-485 operation timed out";

        case EMPTY_BUFFER:
            return "The received buffer is empty or incomplete";

        case WRONG_PARAM:
            return "Invalid parameters were provided to the driver";

        default:
            return "Unknown RS-485 driver error";
    }
}

std::string grpcErrorMessage(
    const grpc::Status &status)
{
    std::string message =
        "gRPC error " +
        std::to_string(
            static_cast<int>(status.error_code())
        );

    if (!status.error_message().empty())
    {
        message += ": ";
        message += status.error_message();
    }

    return message;
}

} // namespace

Rs485Exception::Rs485Exception(
    const std::string &message)
    : std::runtime_error(message)
{
}

Rs485ValidationException::Rs485ValidationException(
    const std::string &message)
    : Rs485Exception(message)
{
}

Rs485ConnectionException::Rs485ConnectionException(
    const std::string &message)
    : Rs485Exception(message)
{
}

Rs485StreamException::Rs485StreamException(
    const std::string &message)
    : Rs485Exception(message)
{
}

Rs485DriverException::Rs485DriverException(
    rs485::driver::v1::PultErrors error)
    : Rs485Exception(driverErrorMessage(error)),
      error_code_(error)
{
}

rs485::driver::v1::PultErrors
Rs485DriverException::errorCode() const noexcept
{
    return error_code_;
}

Rs485GrpcException::Rs485GrpcException(
    const grpc::Status &status)
    : Rs485Exception(grpcErrorMessage(status)),
      status_code_(status.error_code())
{
}

grpc::StatusCode
Rs485GrpcException::statusCode() const noexcept
{
    return status_code_;
}
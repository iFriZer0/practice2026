#ifndef RS485_ERRORS_H__
#define RS485_ERRORS_H__

#include <string>

#include <grpcpp/support/status.h>

#include "../driver/rs485_driver.pb.h"

enum class ClientError
{
    None,

    NotConnected,

    InvalidEndpoint,

    InvalidHex,

    EmptyData,

    EmptyCallback,

    StreamCreationFailed,

    StreamWriteFailed,

    NoResponse,

    Unknown
};

std::string clientErrorToString(ClientError error);

std::string pultErrorToString(
    rs485::driver::v1::PultErrors error
);

std::string grpcStatusToString(
    const grpc::Status &status
);

#endif
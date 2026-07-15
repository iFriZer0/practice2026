#ifndef RS485_ERRORS_H__
#define RS485_ERRORS_H__

#include <stdexcept>
#include <string>

#include <grpcpp/grpcpp.h>

#include "rs485_driver.pb.h"

class Rs485Exception : public std::runtime_error
{
public:
    explicit Rs485Exception(
        const std::string &message
    );

    ~Rs485Exception() noexcept override = default;
};

class Rs485ValidationException final
    : public Rs485Exception
{
public:
    explicit Rs485ValidationException(
        const std::string &message
    );

    ~Rs485ValidationException() noexcept override = default;
};

class Rs485ConnectionException final
    : public Rs485Exception
{
public:
    explicit Rs485ConnectionException(
        const std::string &message
    );

    ~Rs485ConnectionException() noexcept override = default;
};

class Rs485StreamException final
    : public Rs485Exception
{
public:
    explicit Rs485StreamException(
        const std::string &message
    );

    ~Rs485StreamException() noexcept override = default;
};

class Rs485DriverException final
    : public Rs485Exception
{
public:
    explicit Rs485DriverException(
        rs485::driver::v1::PultErrors error
    );

    ~Rs485DriverException() noexcept override = default;

    rs485::driver::v1::PultErrors
    errorCode() const noexcept;

private:
    rs485::driver::v1::PultErrors error_code_;
};

class Rs485GrpcException final
    : public Rs485Exception
{
public:
    explicit Rs485GrpcException(
        const grpc::Status &status
    );

    ~Rs485GrpcException() noexcept override = default;

    grpc::StatusCode
    statusCode() const noexcept;

private:
    grpc::StatusCode status_code_;
};

#endif

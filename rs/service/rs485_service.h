#ifndef RS485_SERVICE_H__
#define RS485_SERVICE_H__

#include <memory>

#include <grpcpp/grpcpp.h>

#include "rs485_service.grpc.pb.h"

class Rs485DriverClient;

class Rs485ServiceImpl final
    : public rs485::service::v1::Rs485Service::Service
{
public:
    explicit Rs485ServiceImpl(
        std::shared_ptr<Rs485DriverClient> driver_client
    );

    ~Rs485ServiceImpl() override = default;

    Rs485ServiceImpl(
        const Rs485ServiceImpl &other
    ) = delete;

    Rs485ServiceImpl(
        Rs485ServiceImpl &&other
    ) = delete;

    Rs485ServiceImpl &operator=(
        const Rs485ServiceImpl &other
    ) = delete;

    Rs485ServiceImpl &operator=(
        Rs485ServiceImpl &&other
    ) = delete;

    grpc::Status SendData(
        grpc::ServerContext *context,
        const rs485::service::v1::SendDataRequest *request,
        rs485::service::v1::SendDataResponse *response
    ) override;

    grpc::Status Subscribe(
        grpc::ServerContext *context,
        const rs485::service::v1::SubscribeRequest *request,
        grpc::ServerWriter<
            rs485::service::v1::ReceiveDataResponse
        > *writer
    ) override;

private:
    std::shared_ptr<Rs485DriverClient> driver_client_;
};

#endif
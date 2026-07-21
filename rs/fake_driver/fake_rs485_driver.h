#ifndef FAKE_RS485_DRIVER_H
#define FAKE_RS485_DRIVER_H

#include <grpcpp/grpcpp.h>

#include "rs485_driver.grpc.pb.h"

class FakeRs485Driver final
    : public rs485::driver::v1::Rs485Driver::Service
{
public:
    grpc::Status SendData(
        grpc::ServerContext* context,
        grpc::ServerReaderWriter<
            rs485::driver::v1::SendDataResponse,
            rs485::driver::v1::SendDataRequest>* stream
    ) override;

    grpc::Status Subscribe(
        grpc::ServerContext* context,
        const google::protobuf::Empty* request,
        grpc::ServerWriter<
            rs485::driver::v1::ReceiveDataResponse>* writer
    ) override;
};

#endif
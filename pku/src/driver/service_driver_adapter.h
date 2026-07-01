#ifndef SERVICE_DRIVER_ADAPTER_H__

#define SERVICE_DRIVER_ADAPTER_H__

#include <grpcpp/support/status.h>
#include <google/protobuf/empty.pb.h>
#include <grpcpp/server_context.h>
#include "service_driver.grpc.pb.h"

class ServiceDriverAdapter final : public grpc_service_driver::ServiceDriver::Service
{
public:
    grpc::Status GetVersion(
            grpc::ServerContext *context,
            const google::protobuf::Empty *request,
            grpc_service_driver::Version *response
    ) noexcept override;
};

#endif

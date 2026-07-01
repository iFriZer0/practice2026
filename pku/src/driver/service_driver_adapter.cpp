#include <grpcpp/support/status.h>
#include <google/protobuf/empty.pb.h>
#include <grpcpp/server_context.h>
#include "service_driver_adapter.h"

grpc::Status ServiceDriverAdapter::GetVersion(
        grpc::ServerContext *context,
        const google::protobuf::Empty *request,
        grpc_service_driver::Version *response
) noexcept
{
    response->set_version("1.0.0");
    return grpc::Status::OK;
}

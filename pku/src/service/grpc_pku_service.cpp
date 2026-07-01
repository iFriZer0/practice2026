#include <grpcpp/support/status.h>
#include <google/protobuf/empty.pb.h>
#include <grpcpp/server_context.h>
#include "grpc_pku_service.h"

grpc::Status GRPCPkuService::GetVersion(grpc::ServerContext *context, const google::protobuf::Empty *request, Version *reply) noexcept
{
    reply->set_version("1.0.0");
    return grpc::Status::OK;
}

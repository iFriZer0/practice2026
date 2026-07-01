#ifndef GRPC_PKU_SERVICE_H__

#define GRPC_PKU_SERVICE_H__

#include <grpcpp/support/status.h>
#include <google/protobuf/empty.pb.h>
#include <grpcpp/server_context.h>
#include "pku_service.grpc.pb.h"

class GRPCPkuService final : public PkuService::Service
{
    grpc::Status GetVersion(grpc::ServerContext *context, const google::protobuf::Empty *request, Version *reply) noexcept override;
};

#endif

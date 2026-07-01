#ifndef GRPC_PKU_SERVICE_H__

#define GRPC_PKU_SERVICE_H__

#include <string>
#include <grpcpp/support/status.h>
#include <google/protobuf/empty.pb.h>
#include <grpcpp/server_context.h>
#include "ui_service.grpc.pb.h"

class UIServiceAdapter final : public grpc_ui_service::UIService::Service
{
public:
    grpc::Status GetVersion(
            grpc::ServerContext *context,
            const google::protobuf::Empty *request,
            grpc_ui_service::Version *reply
    ) noexcept override;
private:
    static const std::string IP;
    static const std::string PORT;
};

#endif

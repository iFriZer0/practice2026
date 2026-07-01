#include <memory>
#include <google/protobuf/empty.pb.h>
#include <grpcpp/support/status.h>
#include <grpcpp/server_context.h>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/client_context.h>
#include "ui_service_adapter.h"
#include "service_driver.grpc.pb.h"
#include "service_driver.pb.h"

const std::string UIServiceAdapter::IP{"localhost"};
const std::string UIServiceAdapter::PORT{":50051"};

grpc::Status UIServiceAdapter::GetVersion(
        grpc::ServerContext *context,
        const google::protobuf::Empty *request,
        grpc_ui_service::Version *reply
) noexcept
{
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(IP + PORT, grpc::InsecureChannelCredentials());
    std::unique_ptr<grpc_service_driver::ServiceDriver::Stub> stub = grpc_service_driver::ServiceDriver::NewStub(channel);
    grpc::ClientContext service_context;
    google::protobuf::Empty service_request;
    grpc_service_driver::Version service_version;
    grpc::Status status = stub->GetVersion(&service_context, service_request, &service_version);
    if (status.ok())
    {
        reply->set_version(service_version.version());
    }
    return status;
}

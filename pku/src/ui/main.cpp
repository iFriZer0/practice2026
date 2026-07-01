#include <iostream>
#include <memory>
#include <string>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/client_context.h>
#include <grpcpp/support/status.h>
#include "ui_service.grpc.pb.h"
#include "ui_service.pb.h"

static const std::string IP{"localhost"};
static const std::string PORT{":50052"};

int main()
{
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel(IP + PORT, grpc::InsecureChannelCredentials());
    std::unique_ptr<grpc_ui_service::UIService::Stub> stub = grpc_ui_service::UIService::NewStub(channel);
    grpc::ClientContext context;
    google::protobuf::Empty request;
    grpc_ui_service::Version reply;
    grpc::Status status = stub->GetVersion(&context, request, &reply);
    if (status.ok())
    {
        std::cout << "Версия: " << reply.version() << std::endl;
    }
    else
    {
        std::cout << "Ошибка: " << status.error_message() << std::endl;
    }
    return 0;
}

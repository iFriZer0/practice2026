#include <iostream>
#include <memory>
#include <grpcpp/channel.h>
#include <grpcpp/create_channel.h>
#include <grpcpp/security/credentials.h>
#include <grpcpp/client_context.h>
#include <grpcpp/support/status.h>
#include "ui_service.grpc.pb.h"
#include "ui_service.pb.h"

int main()
{
    std::shared_ptr<grpc::Channel> channel = grpc::CreateChannel("localhost:50051", grpc::InsecureChannelCredentials());
    std::unique_ptr<UIService::Stub> stub = UIService::NewStub(channel);
    grpc::ClientContext context;
    google::protobuf::Empty request;
    Version reply;
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

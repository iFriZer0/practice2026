#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include "pku_driver_plug.h"

static const std::string IP{"0.0.0.0"};
static const std::string PORT{":50051"};

int main()
{
    grpc::ServerBuilder builder;
    builder.AddListeningPort(IP + PORT, grpc::InsecureServerCredentials());
    PkuDriverPlug driver;
    builder.RegisterService(&driver);
    std::unique_ptr<grpc::Server> server{builder.BuildAndStart()};
    server->Wait();
    return 0;
}

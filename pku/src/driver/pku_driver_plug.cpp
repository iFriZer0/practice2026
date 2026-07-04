#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include "pku_driver_plug.h"
#include "pku_driver_test.pb.h"

grpc::Status PkuDriverPlug::Add(
        grpc::ServerContext *context,
        const pku::driver::v1::AddRequest *request,
        pku::driver::v1::AddResponse *response
) noexcept
{
    response->set_sum(request->number_1() + request->number_2());
    return grpc::Status::OK;
}

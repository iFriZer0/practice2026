#ifndef PKU_DRIVER_PLUG_H__

#define PKU_DRIVER_PLUG_H__

#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include "pku_driver_test.grpc.pb.h"
#include "pku_driver_test.pb.h"

class PkuDriverPlug final : public pku::driver::v1::PkuDriver::Service
{
public:
    grpc::Status Add(
            grpc::ServerContext *context,
            const pku::driver::v1::AddRequest *request,
            pku::driver::v1::AddResponse *response
    ) noexcept override;
};

#endif

#ifndef GRPC_MKO_CLIENT_H__
#define GRPC_MKO_CLIENT_H__

#include <memory>
#include <string>
#include <grpcpp/grpcpp.h>
#include "mko.grpc.pb.h"
#include "mko_client.h"

class GrpcMkoClient final : public MkoClient
{
public:
    explicit GrpcMkoClient(const std::string &server_address);
    ~GrpcMkoClient() override = default;

    GrpcMkoClient(const GrpcMkoClient &other) = delete;
    GrpcMkoClient(GrpcMkoClient &&other) = delete;
    GrpcMkoClient &operator=(const GrpcMkoClient &other) = delete;
    GrpcMkoClient &operator=(GrpcMkoClient &&other) = delete;

    void configure_kk(const ConfigureKKRequestData &request) override;
    void configure_exchange(const ConfigureExchangeRequestData &request) override;
    void run_exchange(const RunExchangeRequestData &request) override;

private:
    std::unique_ptr<mko::workstation::v1::MkoWorkstationService::Stub> stub;

    static void throw_if_not_ok(const grpc::Status &status, const std::string &operation_name);
};

#endif

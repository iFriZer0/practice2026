#include <stdexcept>
#include <utility>
#include "grpc_mko_client.h"

namespace
{
using MkoApi = mko::workstation::v1::MkoWorkstationService;
}

GrpcMkoClient::GrpcMkoClient(const std::string &server_address)
    : stub{MkoApi::NewStub(grpc::CreateChannel(server_address, grpc::InsecureChannelCredentials()))}
{
}

void GrpcMkoClient::configure_kk(const ConfigureKKRequestData &request)
{
    mko::workstation::v1::ConfigureKKRequest proto_request;
    proto_request.set_board_id(request.board_id);
    proto_request.set_index(request.index);
    proto_request.set_channel(request.channel);
    proto_request.set_bus_control(request.bus_control);
    proto_request.set_ou_address(request.ou_address);
    proto_request.set_ou_resp_word(request.ou_resp_word);
    proto_request.set_vector_word(request.vector_word);
    proto_request.set_selftest_word(request.selftest_word);
    proto_request.set_remote_ip(request.remote_ip);
    proto_request.set_remote_port(request.remote_port);
    proto_request.set_operation_id(request.operation_id);

    mko::workstation::v1::Empty response;
    grpc::ClientContext context;
    const grpc::Status status{stub->ConfigureKK(&context, proto_request, &response)};
    throw_if_not_ok(status, "ConfigureKK");
}

void GrpcMkoClient::configure_exchange(const ConfigureExchangeRequestData &request)
{
    mko::workstation::v1::ConfigureExchangeRequest proto_request;
    proto_request.set_board_id(request.board_id);
    proto_request.set_format(request.format);
    proto_request.set_ks1(request.ks1);
    proto_request.set_ks2(request.ks2);
    proto_request.set_operation_id(request.operation_id);

    for (const uint32_t word : request.sd)
    {
        proto_request.add_sd(word);
    }

    mko::workstation::v1::Empty response;
    grpc::ClientContext context;
    const grpc::Status status{stub->ConfigureExchange(&context, proto_request, &response)};
    throw_if_not_ok(status, "ConfigureExchange");
}

void GrpcMkoClient::run_exchange(const RunExchangeRequestData &request)
{
    mko::workstation::v1::RunExchangeRequest proto_request;
    proto_request.set_board_id(request.board_id);
    proto_request.set_operation_id(request.operation_id);

    mko::workstation::v1::Empty response;
    grpc::ClientContext context;
    const grpc::Status status{stub->RunExchange(&context, proto_request, &response)};
    throw_if_not_ok(status, "RunExchange");
}

void GrpcMkoClient::throw_if_not_ok(const grpc::Status &status, const std::string &operation_name)
{
    if (status.ok())
    {
        return;
    }

    throw std::runtime_error{
            operation_name + " failed: " + std::to_string(status.error_code()) + " " + status.error_message()
    };
}

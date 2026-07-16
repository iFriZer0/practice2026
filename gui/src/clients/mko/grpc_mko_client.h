#ifndef GRPC_MKO_CLIENT_H__
#define GRPC_MKO_CLIENT_H__

#include <atomic>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <grpcpp/grpcpp.h>
#include "mko.grpc.pb.h"
#include "mko_client.h"

class GrpcMkoClient final : public MkoClient
{
public:
    explicit GrpcMkoClient(const std::string &server_address);
    ~GrpcMkoClient() override;

    GrpcMkoClient(const GrpcMkoClient &other) = delete;
    GrpcMkoClient(GrpcMkoClient &&other) = delete;
    GrpcMkoClient &operator=(const GrpcMkoClient &other) = delete;
    GrpcMkoClient &operator=(GrpcMkoClient &&other) = delete;

    void configure_kk(const ConfigureKKRequestData &request) override;
    void configure_exchange(const ConfigureExchangeRequestData &request) override;
    void run_exchange(const RunExchangeRequestData &request) override;
    void configure_ou(const ConfigureOuRequestData &request) override;
    void set_ou_response_word(const SetOuResponseWordRequestData &request) override;
    OuSubaddressData read_ou_subaddress(const ReadOuSubaddressRequestData &request) override;
    void write_ou_subaddress(const WriteOuSubaddressRequestData &request) override;
    void send_raw_ou_data(const WriteOuSubaddressRequestData &request) override;
    void clear_receive_buffer(const ClearBufferRequestData &request) override;
    void clear_transmit_buffer(const ClearBufferRequestData &request) override;

    void subscribe_ou_commands(const SubscribeOuCommandsRequestData &request,
                                OuCommandEventHandler on_event,
                                OuSubscriptionErrorHandler on_error) override;
    void unsubscribe_ou_commands() override;

private:
    std::unique_ptr<mko::workstation::v1::MkoWorkstationService::Stub> stub;

    // Состояние фоновой подписки SubscribeOuCommands. mutex защищает
    // subscribe_context/subscribe_thread от гонок между потоком GUI
    // (subscribe_ou_commands/unsubscribe_ou_commands) и деструктором.
    std::mutex subscribe_mutex;
    std::unique_ptr<grpc::ClientContext> subscribe_context;
    std::thread subscribe_thread;
    std::atomic<bool> subscribe_active{false};

    static void throw_if_not_ok(const grpc::Status &status, const std::string &operation_name);
};

#endif

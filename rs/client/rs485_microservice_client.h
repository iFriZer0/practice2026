#ifndef RS485_MICROSERVICE_CLIENT_H__
#define RS485_MICROSERVICE_CLIENT_H__

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <grpcpp/grpcpp.h>

#include "rs485_gui_types.h"
#include "rs485_service.grpc.pb.h"

class Rs485MicroserviceClient
{
public:
    using ReceiveCallback =
        std::function<
            void(const Rs485ReceiveResult &)
        >;

    Rs485MicroserviceClient();

    ~Rs485MicroserviceClient();

    Rs485MicroserviceClient(
        const Rs485MicroserviceClient &other
    ) = delete;

    Rs485MicroserviceClient(
        Rs485MicroserviceClient &&other
    ) = delete;

    Rs485MicroserviceClient &operator=(
        const Rs485MicroserviceClient &other
    ) = delete;

    Rs485MicroserviceClient &operator=(
        Rs485MicroserviceClient &&other
    ) = delete;

    bool connect(
        const std::string &endpoint
    );

    void disconnect();

    bool isConnected() const noexcept;

    Rs485SendResult sendData(
        uint32_t channel_id,
        const std::string &bytes_text
    );

    Rs485SendResult sendData(
        uint32_t channel_id,
        const std::vector<uint8_t> &bytes
    );

    Rs485SendResult sendDataFromFile(
        uint32_t channel_id,
        const std::string &file_path
    );

    void startSubscribe(
        ReceiveCallback callback
    );

    void stopSubscribe();

private:
    std::string endpoint_;

    std::shared_ptr<grpc::Channel> channel_;

    std::unique_ptr<
        rs485::service::v1::Rs485Service::Stub
    > stub_;

    std::atomic<bool> subscribed_{false};

    std::thread subscribe_thread_;

    std::mutex subscribe_mutex_;

    std::unique_ptr<grpc::ClientContext>
        subscribe_context_;

    ReceiveCallback receive_callback_;

    void subscribeLoop();
};

#endif
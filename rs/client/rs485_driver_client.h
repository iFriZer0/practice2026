#ifndef RS485_DRIVER_CLIENT_H__
#define RS485_DRIVER_CLIENT_H__

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "rs485_driver.grpc.pb.h"
#include "rs485_types.h"

class Rs485Subscriber;

class Rs485DriverClient
{
public:
    Rs485DriverClient();
    ~Rs485DriverClient();

    Rs485DriverClient(
        const Rs485DriverClient &other
    ) = delete;

    Rs485DriverClient(
        Rs485DriverClient &&other
    ) = delete;

    Rs485DriverClient &operator=(
        const Rs485DriverClient &other
    ) = delete;

    Rs485DriverClient &operator=(
        Rs485DriverClient &&other
    ) = delete;

    bool connect(
        const std::string &endpoint
    );

    void disconnect();

    bool isConnected() const noexcept;

    const std::string &endpoint() const noexcept;

    SendDataResult sendData(
        uint32_t channel_id,
        const std::string &bytes_text
    );

    void startSubscribe(
        std::function<
            void(const ReceiveDataResult &)
        > callback
    );

    void stopSubscribe();

private:
    std::string endpoint_;

    std::shared_ptr<grpc::Channel> channel_;

    std::unique_ptr<
        rs485::driver::v1::Rs485Driver::Stub
    > stub_;

    std::unique_ptr<Rs485Subscriber> subscriber_;
};

#endif
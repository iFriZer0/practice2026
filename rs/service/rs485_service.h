#ifndef RS485_SERVICE_H__
#define RS485_SERVICE_H__

#include <cstdint>
#include <functional>
#include <memory>
#include <string>

#include <grpcpp/grpcpp.h>

#include "../driver/rs485_driver.grpc.pb.h"
#include "rs485_types.h"

class Rs485Subscriber;

class Rs485Service
{
public:
    Rs485Service();
    ~Rs485Service();

    Rs485Service(const Rs485Service &other) = delete;
    Rs485Service(Rs485Service &&other) = delete;

    Rs485Service &operator=(const Rs485Service &other) = delete;
    Rs485Service &operator=(Rs485Service &&other) = delete;

    bool connect(const std::string &endpoint);

    bool isConnected() const;

    SendDataResult sendData(
        uint32_t channel_id,
        const std::string &bytes_text
    );

    void startSubscribe(
        std::function<void(const ReceiveDataResult &)> callback
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
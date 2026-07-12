#ifndef RS485_SUBSCRIBER_H__
#define RS485_SUBSCRIBER_H__

#include <atomic>
#include <functional>
#include <memory>
#include <thread>

#include <google/protobuf/empty.pb.h>
#include <grpcpp/grpcpp.h>

#include "../driver/rs485_driver.grpc.pb.h"
#include "rs485_types.h"

class Rs485Subscriber
{
public:
    using Stub =
        rs485::driver::v1::Rs485Driver::Stub;

    using Callback =
        std::function<
            void(const ReceiveDataResult &)
        >;

    Rs485Subscriber();
    ~Rs485Subscriber();

    Rs485Subscriber(
        const Rs485Subscriber &other
    ) = delete;

    Rs485Subscriber(
        Rs485Subscriber &&other
    ) = delete;

    Rs485Subscriber &operator=(
        const Rs485Subscriber &other
    ) = delete;

    Rs485Subscriber &operator=(
        Rs485Subscriber &&other
    ) = delete;

    void start(
        Stub *stub,
        Callback callback
    );

    void stop();

    bool isRunning() const noexcept;

private:
    void receiveLoop();

    void notify(
        const ReceiveDataResult &result
    ) noexcept;

    Stub *stub_{nullptr};

    std::unique_ptr<grpc::ClientContext> context_;

    std::thread worker_;

    std::atomic_bool running_{false};

    Callback callback_;
};

#endif
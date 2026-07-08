#include "rs485_subscriber.h"

#include <stdexcept>
#include <utility>

void Rs485Subscriber::start(
    Stub *stub,
    std::function<void(const ReceiveDataResult &)> callback)
{
    if (running_)
    {
        return;
    }

    if (stub == nullptr)
    {
        throw std::invalid_argument("Stub is null.");
    }

    stub_ = stub;

    callback_ = std::move(callback);

    context_ = std::make_unique<grpc::ClientContext>();

    running_ = true;

    worker_ = std::thread(
        &Rs485Subscriber::receiveLoop,
        this
    );
}

void Rs485Subscriber::stop()
{
    if (!running_)
    {
        return;
    }

    running_ = false;

    if (context_)
    {
        context_->TryCancel();
    }

    if (worker_.joinable())
    {
        worker_.join();
    }

    context_.reset();
}

Rs485Subscriber::Rs485Subscriber() = default;

Rs485Subscriber::~Rs485Subscriber()
{
    stop();
}

bool Rs485Subscriber::isRunning() const
{
    return running_;
}
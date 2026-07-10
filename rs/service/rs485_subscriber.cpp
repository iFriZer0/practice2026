#include "rs485_subscriber.h"

#include "rs485_errors.h"

#include <stdexcept>
#include <string>
#include <utility>

Rs485Subscriber::Rs485Subscriber() = default;

Rs485Subscriber::~Rs485Subscriber()
{
    stop();
}

void Rs485Subscriber::start(
    Stub *stub,
    std::function<void(const ReceiveDataResult &)> callback)
{
    if (running_)
    {
        return;
    }

    if (worker_.joinable())
    {
        worker_.join();
    }
    
    if (stub == nullptr)
    {
        throw std::invalid_argument(
            clientErrorToString(ClientError::NotConnected)
        );
    }

    if (!callback)
    {
        throw std::invalid_argument(
            clientErrorToString(ClientError::EmptyCallback)
        );
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
    stub_ = nullptr;
    callback_ = {};
}

bool Rs485Subscriber::isRunning() const
{
    return running_;
}

void Rs485Subscriber::receiveLoop()
{
    google::protobuf::Empty request;

    auto reader = stub_->Subscribe(
        context_.get(),
        request
    );

    if (!reader)
    {
        running_ = false;

        if (callback_)
        {
            ReceiveDataResult result;
            result.success = false;
            result.error_message =
                clientErrorToString(ClientError::StreamCreationFailed);

            callback_(result);
        }

        return;
    }

    rs485::driver::v1::ReceiveDataResponse response;

    while (running_ && reader->Read(&response))
    {
        ReceiveDataResult result;

        result.success = response.success();
        result.channel_id = response.channel_id();
        result.error_message =
            pultErrorToString(response.error_message());

        result.packets.reserve(
            static_cast<std::size_t>(response.data_size())
        );

        for (const std::string &raw_packet : response.data())
        {
            ReceiveDataPacket packet;

            packet.bytes.assign(
                raw_packet.begin(),
                raw_packet.end()
            );

            result.packets.push_back(std::move(packet));
        }

        if (callback_)
        {
            callback_(result);
        }

        response.Clear();
    }

    const grpc::Status status = reader->Finish();

    running_ = false;

    if (!status.ok() &&
        status.error_code() != grpc::StatusCode::CANCELLED &&
        callback_)
    {
        ReceiveDataResult result;

        result.success = false;
        result.error_message = grpcStatusToString(status);

        callback_(result);
    }
}
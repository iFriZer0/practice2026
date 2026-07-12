#include "rs485_subscriber.h"

#include "rs485_errors.h"

#include <cstddef>
#include <exception>
#include <string>
#include <utility>

Rs485Subscriber::Rs485Subscriber() = default;

Rs485Subscriber::~Rs485Subscriber()
{
    stop();
}

void Rs485Subscriber::start(
    Stub *stub,
    Callback callback)
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
        throw Rs485ConnectionException(
            "The RS-485 gRPC stub is null"
        );
    }

    if (!callback)
    {
        throw Rs485ValidationException(
            "No receive callback was provided"
        );
    }

    stub_ = stub;
    callback_ = std::move(callback);

    context_ =
        std::make_unique<grpc::ClientContext>();

    running_ = true;

    try
    {
        worker_ = std::thread(
            &Rs485Subscriber::receiveLoop,
            this
        );
    }
    catch (...)
    {
        running_ = false;
        context_.reset();
        stub_ = nullptr;
        callback_ = {};

        throw;
    }
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

bool Rs485Subscriber::isRunning() const noexcept
{
    return running_;
}

void Rs485Subscriber::notify(
    const ReceiveDataResult &result) noexcept
{
    if (!callback_)
    {
        return;
    }

    try
    {
        callback_(result);
    }
    catch (...)
    {
    }
}

void Rs485Subscriber::receiveLoop()
{
    try
    {
        google::protobuf::Empty request;

        auto reader = stub_->Subscribe(
            context_.get(),
            request
        );

        if (!reader)
        {
            throw Rs485StreamException(
                "Failed to create the Subscribe "
                "gRPC stream"
            );
        }

        rs485::driver::v1::ReceiveDataResponse
            response;

        while (running_ &&
               reader->Read(&response))
        {
            ReceiveDataResult result;

            result.channel_id =
                response.channel_id();

            try
            {
                if (!response.success() ||
                    response.error_message() !=
                        rs485::driver::v1::NO_ERROR)
                {
                    throw Rs485DriverException(
                        response.error_message()
                    );
                }

                result.success = true;

                result.packets.reserve(
                    static_cast<std::size_t>(
                        response.data_size()
                    )
                );

                for (const std::string &raw_packet :
                     response.data())
                {
                    ReceiveDataPacket packet;

                    packet.bytes.assign(
                        raw_packet.begin(),
                        raw_packet.end()
                    );

                    result.packets.push_back(
                        std::move(packet)
                    );
                }
            }
            catch (const Rs485Exception &error)
            {
                result.success = false;
                result.error_message = error.what();
            }

            notify(result);

            response.Clear();
        }

        const grpc::Status status =
            reader->Finish();

        if (!status.ok() &&
            status.error_code() !=
                grpc::StatusCode::CANCELLED)
        {
            throw Rs485GrpcException(status);
        }
    }
    catch (const Rs485Exception &error)
    {
        ReceiveDataResult result;

        result.success = false;
        result.error_message = error.what();

        notify(result);
    }
    catch (const std::exception &error)
    {
        ReceiveDataResult result;

        result.success = false;
        result.error_message =
            std::string(
                "Unexpected subscription error: "
            ) +
            error.what();

        notify(result);
    }
    catch (...)
    {
        ReceiveDataResult result;

        result.success = false;
        result.error_message =
            "Unknown subscription error";

        notify(result);
    }

    running_ = false;
}
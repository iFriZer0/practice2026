#include "rs485_driver_client.h"

#include "rs485_errors.h"
#include "rs485_subscriber.h"
#include "rs485_utils.h"

#include <exception>
#include <string>
#include <utility>
#include <vector>

Rs485DriverClient::Rs485DriverClient()
    : subscriber_{
          std::make_unique<Rs485Subscriber>()
      }
{
}

Rs485DriverClient::~Rs485DriverClient()
{
    stopSubscribe();
}

bool Rs485DriverClient::connect(
    const std::string &endpoint)
{
    if (endpoint.empty())
    {
        throw Rs485ValidationException(
            "The RS-485 driver endpoint is empty"
        );
    }

    stopSubscribe();

    try
    {
        std::shared_ptr<grpc::Channel> new_channel =
            grpc::CreateChannel(
                endpoint,
                grpc::InsecureChannelCredentials()
            );

        if (!new_channel)
        {
            throw Rs485ConnectionException(
                "Failed to create the RS-485 gRPC channel"
            );
        }

        std::unique_ptr<
            rs485::driver::v1::Rs485Driver::Stub
        > new_stub =
            rs485::driver::v1::Rs485Driver::NewStub(
                new_channel
            );

        if (!new_stub)
        {
            throw Rs485ConnectionException(
                "Failed to create the RS-485 driver stub"
            );
        }

        endpoint_ = endpoint;
        channel_ = std::move(new_channel);
        stub_ = std::move(new_stub);

        return true;
    }
    catch (const Rs485Exception &)
    {
        endpoint_.clear();
        channel_.reset();
        stub_.reset();

        throw;
    }
    catch (const std::exception &exception)
    {
        endpoint_.clear();
        channel_.reset();
        stub_.reset();

        throw Rs485ConnectionException(
            std::string{
                "Failed to connect to the RS-485 driver: "
            } +
            exception.what()
        );
    }
    catch (...)
    {
        endpoint_.clear();
        channel_.reset();
        stub_.reset();

        throw Rs485ConnectionException(
            "Unknown error while connecting "
            "to the RS-485 driver"
        );
    }
}

bool Rs485DriverClient::isConnected() const noexcept
{
    return channel_ != nullptr &&
           stub_ != nullptr;
}

SendDataResult Rs485DriverClient::sendData(
    uint32_t channel_id,
    const std::string &bytes_text)
{
    if (bytes_text.empty())
    {
        throw Rs485ValidationException(
            "Byte sequence is empty"
        );
    }

    const std::vector<uint8_t> bytes =
        parseBytes(bytes_text);

    return sendData(
        channel_id,
        bytes
    );
}

SendDataResult Rs485DriverClient::sendData(
    uint32_t channel_id,
    const std::vector<uint8_t> &bytes)
{
    if (!isConnected())
    {
        throw Rs485ConnectionException(
            "The RS-485 driver client is not connected"
        );
    }

    if (bytes.empty())
    {
        throw Rs485ValidationException(
            "Byte sequence is empty"
        );
    }

    const std::string raw_data(
        reinterpret_cast<const char *>(
            bytes.data()
        ),
        bytes.size()
    );

    grpc::ClientContext context;

    auto stream = stub_->SendData(&context);

    if (!stream)
    {
        throw Rs485StreamException(
            "Failed to create the SendData gRPC stream"
        );
    }

    rs485::driver::v1::SendDataRequest request;

    request.set_channel_id(channel_id);
    request.set_data(raw_data);

    if (!stream->Write(request))
    {
        stream->WritesDone();

        const grpc::Status status =
            stream->Finish();

        if (!status.ok())
        {
            throw Rs485GrpcException(status);
        }

        throw Rs485StreamException(
            "Failed to write SendDataRequest "
            "to the gRPC stream"
        );
    }

    stream->WritesDone();

    rs485::driver::v1::SendDataResponse response;

    const bool response_received =
        stream->Read(&response);

    const grpc::Status status =
        stream->Finish();

    if (!status.ok())
    {
        throw Rs485GrpcException(status);
    }

    if (!response_received)
    {
        throw Rs485StreamException(
            "The RS-485 driver did not return "
            "a SendData response"
        );
    }

    if (!response.success() ||
        response.error_message() !=
            rs485::driver::v1::NO_ERROR)
    {
        throw Rs485DriverException(
            response.error_message()
        );
    }

    SendDataResult result;

    result.success = true;
    result.channel_id =
        response.channel_id();

    result.error_message.clear();

    return result;
}

void Rs485DriverClient::startSubscribe(
    std::function<
        void(const ReceiveDataResult &)
    > callback)
{
    if (!isConnected())
    {
        throw Rs485ConnectionException(
            "The RS-485 driver client is not connected"
        );
    }

    if (!callback)
    {
        throw Rs485ValidationException(
            "No receive callback was provided"
        );
    }

    try
    {
        subscriber_->start(
            stub_.get(),
            std::move(callback)
        );
    }
    catch (const Rs485Exception &)
    {
        throw;
    }
    catch (const std::exception &exception)
    {
        throw Rs485StreamException(
            std::string{
                "Failed to start the RS-485 subscription: "
            } +
            exception.what()
        );
    }
    catch (...)
    {
        throw Rs485StreamException(
            "Unknown error while starting "
            "the RS-485 subscription"
        );
    }
}

void Rs485DriverClient::stopSubscribe()
{
    if (!subscriber_)
    {
        return;
    }

    subscriber_->stop();
}


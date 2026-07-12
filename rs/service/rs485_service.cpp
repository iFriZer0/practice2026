#include "rs485_service.h"

#include "rs485_errors.h"
#include "rs485_subscriber.h"
#include "rs485_utils.h"

#include <string>
#include <utility>
#include <vector>

Rs485Service::Rs485Service()
    : subscriber_{
          std::make_unique<Rs485Subscriber>()
      }
{
}

Rs485Service::~Rs485Service() = default;

bool Rs485Service::connect(
    const std::string &endpoint)
{
    if (endpoint.empty())
    {
        throw Rs485ValidationException(
            "The RS-485 driver endpoint is empty"
        );
    }

    stopSubscribe();

    endpoint_ = endpoint;

    channel_ = grpc::CreateChannel(
        endpoint_,
        grpc::InsecureChannelCredentials()
    );

    if (!channel_)
    {
        throw Rs485ConnectionException(
            "Failed to create the gRPC channel"
        );
    }

    stub_ =
        rs485::driver::v1::Rs485Driver::NewStub(
            channel_
        );

    if (!stub_)
    {
        channel_.reset();

        throw Rs485ConnectionException(
            "Failed to create the RS-485 gRPC stub"
        );
    }

    return true;
}

bool Rs485Service::isConnected() const noexcept
{
    return static_cast<bool>(stub_);
}

SendDataResult Rs485Service::sendData(
    uint32_t channel_id,
    const std::string &bytes_text)
{
    if (!isConnected())
    {
        throw Rs485ConnectionException(
            "The RS-485 service is not connected"
        );
    }

    /*
     * parseBytes() самостоятельно бросит
     * Rs485ValidationException при неверных данных.
     */
    const std::vector<uint8_t> bytes =
        parseBytes(bytes_text);

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

    rs485::driver::v1::SendDataResponce response;

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
    result.channel_id = response.channel_id();
    result.error_message.clear();

    return result;
}

void Rs485Service::startSubscribe(
    std::function<
        void(const ReceiveDataResult &)
    > callback)
{
    if (!isConnected())
    {
        throw Rs485ConnectionException(
            "The RS-485 service is not connected"
        );
    }

    if (!callback)
    {
        throw Rs485ValidationException(
            "No receive callback was provided"
        );
    }

    subscriber_->start(
        stub_.get(),
        std::move(callback)
    );
}

void Rs485Service::stopSubscribe()
{
    if (subscriber_)
    {
        subscriber_->stop();
    }
}
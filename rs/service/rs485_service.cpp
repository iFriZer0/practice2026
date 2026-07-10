#include "rs485_service.h"

#include "rs485_errors.h"
#include "rs485_subscriber.h"
#include "rs485_utils.h"

#include <stdexcept>
#include <utility>
#include <vector>

Rs485Service::Rs485Service()
    : subscriber_(std::make_unique<Rs485Subscriber>())
{
}

Rs485Service::~Rs485Service() = default;

bool Rs485Service::connect(const std::string &endpoint)
{
    if (endpoint.empty())
    {
        throw std::invalid_argument(
            clientErrorToString(ClientError::InvalidEndpoint)
        );
    }

    stopSubscribe();

    endpoint_ = endpoint;

    channel_ = grpc::CreateChannel(
        endpoint_,
        grpc::InsecureChannelCredentials()
    );

    stub_ =
        rs485::driver::v1::Rs485Driver::NewStub(channel_);

    return isConnected();
}

bool Rs485Service::isConnected() const
{
    return channel_ != nullptr && stub_ != nullptr;
}

SendDataResult Rs485Service::sendData(
    uint32_t channel_id,
    const std::string &bytes_text)
{
    SendDataResult result;
    result.channel_id = channel_id;

    if (!isConnected())
    {
        result.success = false;
        result.error_message =
            clientErrorToString(ClientError::NotConnected);

        return result;
    }

    if (bytes_text.empty())
    {
        result.success = false;
        result.error_message =
            clientErrorToString(ClientError::EmptyData);

        return result;
    }

    std::vector<uint8_t> bytes;

    try
    {
        bytes = parseBytes(bytes_text);
    }
    catch (const std::invalid_argument &)
    {
        result.success = false;
        result.error_message =
            clientErrorToString(ClientError::InvalidHex);

        return result;
    }
    catch (const std::out_of_range &)
    {
        result.success = false;
        result.error_message =
            clientErrorToString(ClientError::InvalidHex);

        return result;
    }

    if (bytes.empty())
    {
        result.success = false;
        result.error_message =
            clientErrorToString(ClientError::EmptyData);

        return result;
    }

    const std::string raw_data(
        reinterpret_cast<const char *>(bytes.data()),
        bytes.size()
    );

    grpc::ClientContext context;

    auto stream = stub_->SendData(&context);

    if (!stream)
    {
        result.success = false;
        result.error_message =
            clientErrorToString(
                ClientError::StreamCreationFailed
            );

        return result;
    }

    rs485::driver::v1::SendDataRequest request;

    request.set_channel_id(channel_id);
    request.set_data(raw_data);

    if (!stream->Write(request))
    {
        stream->WritesDone();

        const grpc::Status status = stream->Finish();

        result.success = false;

        if (!status.ok())
        {
            result.error_message =
                grpcStatusToString(status);
        }
        else
        {
            result.error_message =
                clientErrorToString(
                    ClientError::StreamWriteFailed
                );
        }

        return result;
    }

    stream->WritesDone();

    rs485::driver::v1::SendDataResponce response;

    const bool response_received =
        stream->Read(&response);

    const grpc::Status status = stream->Finish();

    if (!status.ok())
    {
        result.success = false;
        result.error_message =
            grpcStatusToString(status);

        return result;
    }

    if (!response_received)
    {
        result.success = false;
        result.error_message =
            clientErrorToString(ClientError::NoResponse);

        return result;
    }

    result.success = response.success();
    result.channel_id = response.channel_id();
    result.error_message =
        pultErrorToString(response.error_message());

    return result;
}

void Rs485Service::startSubscribe(
    std::function<void(const ReceiveDataResult &)> callback)
{
    if (!isConnected())
    {
        throw std::runtime_error(
            clientErrorToString(ClientError::NotConnected)
        );
    }

    if (!callback)
    {
        throw std::invalid_argument(
            clientErrorToString(ClientError::EmptyCallback)
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
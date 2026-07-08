#include "rs485_service.h"
#include "rs485_utils.h"
#include <stdexcept>

SendDataResult Rs485Service::sendData(
    uint32_t channel_id,
    const std::string &bytes_text)
{
    if (!isConnected())
    {
        throw std::runtime_error("RS-485 service is not connected");
    }

    SendDataResult result;
    result.channel_id = channel_id;

    const std::vector<uint8_t> bytes = parseBytes(bytes_text);

    std::string raw_data(
        reinterpret_cast<const char *>(bytes.data()),
        bytes.size()
    );

    grpc::ClientContext context;

    auto stream = stub_->SendData(&context);

    rs485::driver::v1::SendDataRequest request;
    request.set_channel_id(channel_id);
    request.set_data(raw_data);

    if (!stream->Write(request))
    {
        result.success = false;
        result.error_message = "Failed to write SendDataRequest to stream";
        return result;
    }

    stream->WritesDone();

    rs485::driver::v1::SendDataResponce response;

    if (stream->Read(&response))
    {
        result.success = response.success();
        result.channel_id = response.channel_id();
        result.error_message =
            rs485::driver::v1::PultErrors_Name(response.error_message());
    }
    else
    {
        result.success = false;
        result.error_message = "No SendData response from driver";
    }

    grpc::Status status = stream->Finish();

    if (!status.ok())
    {
        result.success = false;
        result.error_message =
            "gRPC error: " + status.error_message();
    }

    return result;
}

bool Rs485Service::connect(const std::string &endpoint)
{
    endpoint_ = endpoint;

    channel_ = grpc::CreateChannel(
        endpoint_,
        grpc::InsecureChannelCredentials()
    );

    stub_ = rs485::driver::v1::Rs485Driver::NewStub(channel_);

    return isConnected();
}

bool Rs485Service::isConnected() const
{
    return static_cast<bool>(stub_);
}

void Rs485Service::startSubscribe(
    std::function<void(const ReceiveDataResult &)> callback)
{
    subscriber_->start(stub_.get(), callback);
}

void Rs485Service::stopSubscribe()
{
    subscriber_->stop();
}
#include "rs485_microservice_client.h"

#include <cctype>
#include <exception>
#include <fstream>
#include <iterator>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

Rs485MicroserviceClient::Rs485MicroserviceClient() = default;

Rs485MicroserviceClient::~Rs485MicroserviceClient()
{
    disconnect();
}

bool Rs485MicroserviceClient::connect(
    const std::string &endpoint)
{
    if (endpoint.empty())
    {
        throw std::invalid_argument(
            "The RS-485 microservice endpoint is empty"
        );
    }

    disconnect();

    std::shared_ptr<grpc::Channel> new_channel =
        grpc::CreateChannel(
            endpoint,
            grpc::InsecureChannelCredentials()
        );

    if (!new_channel)
    {
        throw std::runtime_error(
            "Failed to create the RS-485 "
            "microservice gRPC channel"
        );
    }

    std::unique_ptr<
        rs485::service::v1::Rs485Service::Stub
    > new_stub =
        rs485::service::v1::Rs485Service::NewStub(
            new_channel
        );

    if (!new_stub)
    {
        throw std::runtime_error(
            "Failed to create the RS-485 "
            "microservice gRPC stub"
        );
    }

    endpoint_ = endpoint;
    channel_ = std::move(new_channel);
    stub_ = std::move(new_stub);

    return true;
}

void Rs485MicroserviceClient::disconnect()
{
    stopSubscribe();

    stub_.reset();
    channel_.reset();
    endpoint_.clear();
}

bool Rs485MicroserviceClient::isConnected() const noexcept
{
    return channel_ != nullptr &&
           stub_ != nullptr;
}

Rs485SendResult Rs485MicroserviceClient::sendData(
    uint32_t channel_id,
    const std::string &bytes_text)
{
    std::istringstream stream{
        bytes_text
    };

    std::string token;
    std::vector<uint8_t> bytes;

    while (stream >> token)
    {
        if (token.size() != 2)
        {
            throw std::invalid_argument(
                "Each byte must contain exactly "
                "two hexadecimal digits"
            );
        }

        if (!std::isxdigit(
                static_cast<unsigned char>(
                    token[0]
                )
            ) ||
            !std::isxdigit(
                static_cast<unsigned char>(
                    token[1]
                )
            ))
        {
            throw std::invalid_argument(
                "The data contains an invalid "
                "hexadecimal byte"
            );
        }

        const unsigned long value =
            std::stoul(
                token,
                nullptr,
                16
            );

        if (value > 0xFFUL)
        {
            throw std::out_of_range(
                "The hexadecimal byte is out of range"
            );
        }

        bytes.push_back(
            static_cast<uint8_t>(
                value
            )
        );
    }

    if (bytes.empty())
    {
        throw std::invalid_argument(
            "The RS-485 data field is empty"
        );
    }

    return sendData(
        channel_id,
        bytes
    );
}

Rs485SendResult Rs485MicroserviceClient::sendData(
    uint32_t channel_id,
    const std::vector<uint8_t> &bytes)
{
    if (!isConnected())
    {
        throw std::runtime_error(
            "The RS-485 microservice client is not connected"
        );
    }

    if (bytes.empty())
    {
        throw std::invalid_argument(
            "Byte sequence is empty"
        );
    }

    const std::string binary_data(
        reinterpret_cast<const char *>(bytes.data()),
        bytes.size()
    );

    rs485::service::v1::SendDataRequest request;

    request.set_channel_id(channel_id);
    request.set_data(binary_data);

    rs485::service::v1::SendDataResponse response;

    grpc::ClientContext context;

    const grpc::Status status =
        stub_->SendData(
            &context,
            request,
            &response
        );

    if (!status.ok())
    {
        throw std::runtime_error(
            "RS-485 microservice gRPC error: " +
            status.error_message()
        );
    }

    Rs485SendResult result;

    result.success = response.success();
    result.channel_id = response.channel_id();
    result.error_message = response.error_message();

    return result;
}

Rs485SendResult Rs485MicroserviceClient::sendDataFromFile(
    uint32_t channel_id,
    const std::string &file_path)
{
    if (file_path.empty())
    {
        throw std::invalid_argument(
            "File path is empty"
        );
    }

    std::ifstream file(
        file_path,
        std::ios::binary
    );

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Failed to open file: " +
            file_path
        );
    }

    const std::vector<char> raw_data{
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>()
    };

    if (file.bad())
    {
        throw std::runtime_error(
            "Failed to read file: " +
            file_path
        );
    }

    if (raw_data.empty())
    {
        throw std::invalid_argument(
            "Selected file is empty"
        );
    }

    std::vector<uint8_t> bytes;
    bytes.reserve(raw_data.size());

    for (const char value : raw_data)
    {
        bytes.push_back(
            static_cast<uint8_t>(
                static_cast<unsigned char>(
                    value
                )
            )
        );
    }

    return sendData(
        channel_id,
        bytes
    );
}

void Rs485MicroserviceClient::startSubscribe(
    ReceiveCallback callback)
{
    if (!isConnected())
    {
        throw std::runtime_error(
            "The RS-485 microservice client "
            "is not connected"
        );
    }

    if (!callback)
    {
        throw std::invalid_argument(
            "No RS-485 receive callback was provided"
        );
    }

    std::lock_guard<std::mutex> lock{
        subscribe_mutex_
    };

    if (subscribed_)
    {
        return;
    }

    if (subscribe_thread_.joinable())
    {
        subscribe_thread_.join();
    }

    receive_callback_ = std::move(callback);

    subscribe_context_ =
        std::make_unique<grpc::ClientContext>();

    subscribed_ = true;

    try
    {
        subscribe_thread_ = std::thread(
            &Rs485MicroserviceClient::subscribeLoop,
            this
        );
    }
    catch (...)
    {
        subscribed_ = false;
        subscribe_context_.reset();
        receive_callback_ = {};

        throw;
    }
}

void Rs485MicroserviceClient::stopSubscribe()
{
    {
        std::lock_guard<std::mutex> lock{
            subscribe_mutex_
        };

        subscribed_ = false;

        if (subscribe_context_)
        {
            subscribe_context_->TryCancel();
        }
    }

    if (subscribe_thread_.joinable())
    {
        subscribe_thread_.join();
    }

    std::lock_guard<std::mutex> lock{
        subscribe_mutex_
    };

    subscribe_context_.reset();
    receive_callback_ = {};
}

void Rs485MicroserviceClient::subscribeLoop()
{
    Rs485ReceiveResult final_result;

    try
    {
        rs485::service::v1::SubscribeRequest request;

        grpc::ClientReader<
            rs485::service::v1::ReceiveDataResponse
        > *raw_reader = nullptr;

        std::unique_ptr<
            grpc::ClientReader<
                rs485::service::v1::ReceiveDataResponse
            >
        > reader;

        {
            std::lock_guard<std::mutex> lock{
                subscribe_mutex_
            };

            if (!subscribe_context_ || !stub_)
            {
                throw std::runtime_error(
                    "The RS-485 subscription context "
                    "is not initialized"
                );
            }

            reader = stub_->Subscribe(
                subscribe_context_.get(),
                request
            );

            raw_reader = reader.get();
        }

        if (raw_reader == nullptr)
        {
            throw std::runtime_error(
                "Failed to create the RS-485 "
                "microservice subscription stream"
            );
        }

        rs485::service::v1::ReceiveDataResponse
            response;

        while (subscribed_ &&
               raw_reader->Read(&response))
        {
            Rs485ReceiveResult result;

            result.success = response.success();
            result.channel_id = response.channel_id();
            result.error_message =
                response.error_message();

            const std::string &binary_data =
                response.data();

            result.data.assign(
                binary_data.begin(),
                binary_data.end()
            );

            ReceiveCallback callback;

            {
                std::lock_guard<std::mutex> lock{
                    subscribe_mutex_
                };

                callback = receive_callback_;
            }

            if (callback)
            {
                callback(result);
            }

            response.Clear();
        }

        const grpc::Status status =
            raw_reader->Finish();

        if (!status.ok() &&
            status.error_code() !=
                grpc::StatusCode::CANCELLED)
        {
            final_result.success = false;
            final_result.error_message =
                "RS-485 subscription gRPC error: " +
                status.error_message();
        }
    }
    catch (const std::exception &exception)
    {
        final_result.success = false;
        final_result.error_message =
            exception.what();
    }
    catch (...)
    {
        final_result.success = false;
        final_result.error_message =
            "Unknown RS-485 subscription error";
    }

    subscribed_ = false;

    if (!final_result.error_message.empty())
    {
        ReceiveCallback callback;

        {
            std::lock_guard<std::mutex> lock{
                subscribe_mutex_
            };

            callback = receive_callback_;
        }

        if (callback)
        {
            callback(final_result);
        }
    }
}
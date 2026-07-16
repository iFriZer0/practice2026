#include <grpcpp/grpcpp.h>

#include "rs485_service.grpc.pb.h"

#include <cstdint>
#include <exception>
#include <iomanip>
#include <iostream>
#include <memory>
#include <sstream>
#include <string>

namespace
{

std::string bytesToHex(const std::string& data)
{
    std::ostringstream result;

    result << std::hex
           << std::uppercase
           << std::setfill('0');

    for (const unsigned char byte : data)
    {
        result << std::setw(2)
               << static_cast<unsigned int>(byte)
               << ' ';
    }

    return result.str();
}

void testSendData(
    rs485::service::v1::Rs485Service::Stub* stub,
    std::uint32_t channelId,
    const std::string& data)
{
    grpc::ClientContext context;

    rs485::service::v1::SendDataRequest request;
    rs485::service::v1::SendDataResponse response;

    request.set_channel_id(channelId);
    request.set_data(data);

    const grpc::Status status =
        stub->SendData(
            &context,
            request,
            &response
        );

    if (!status.ok())
    {
        std::cerr
            << "Microservice gRPC error\n"
            << "code: "
            << status.error_code()
            << '\n'
            << "message: "
            << status.error_message()
            << '\n';

        return;
    }

    std::cout
        << "\nSendData response\n"
        << "success: "
        << std::boolalpha
        << response.success()
        << '\n'
        << "channel_id: "
        << response.channel_id()
        << '\n'
        << "error_code: "
        << response.error_code()
        << '\n'
        << "error_message: "
        << response.error_message()
        << '\n';
}

void testSubscribe(
    rs485::service::v1::Rs485Service::Stub* stub)
{
    grpc::ClientContext context;

    rs485::service::v1::SubscribeRequest request;
    rs485::service::v1::ReceiveDataResponse response;

    std::unique_ptr<
        grpc::ClientReader<
            rs485::service::v1::ReceiveDataResponse>>
        reader = stub->Subscribe(
            &context,
            request
        );

    std::cout
        << "\nSubscription started\n"
        << "The client will receive 10 messages\n";

    int receivedCount = 0;

    while (reader->Read(&response))
    {
        std::cout
            << "\nReceived packet\n"
            << "success: "
            << std::boolalpha
            << response.success()
            << '\n'
            << "channel_id: "
            << response.channel_id()
            << '\n'
            << "data: "
            << bytesToHex(response.data())
            << '\n'
            << "error_code: "
            << response.error_code()
            << '\n'
            << "error_message: "
            << response.error_message()
            << '\n';

        ++receivedCount;

        if (receivedCount >= 10)
        {
            std::cout
                << "\nCancelling subscription...\n";

            context.TryCancel();
            break;
        }
    }

    const grpc::Status status =
        reader->Finish();

    if (status.ok())
    {
        std::cout
            << "Subscription finished successfully\n";
    }
    else if (
        status.error_code() ==
        grpc::StatusCode::CANCELLED)
    {
        std::cout
            << "Subscription was cancelled by client\n";
    }
    else
    {
        std::cerr
            << "Subscription finished with error\n"
            << "code: "
            << status.error_code()
            << '\n'
            << "message: "
            << status.error_message()
            << '\n';
    }
}

} // namespace

int main(int argc, char* argv[])
{
    try
    {
        std::string endpoint = "127.0.0.1:50052";

        if (argc > 1)
        {
            endpoint = argv[1];
        }

        const std::shared_ptr<grpc::Channel> channel =
            grpc::CreateChannel(
                endpoint,
                grpc::InsecureChannelCredentials()
            );

        std::unique_ptr<
            rs485::service::v1::Rs485Service::Stub>
            stub =
                rs485::service::v1::Rs485Service::NewStub(
                    channel
                );

        std::cout
            << "Connected to microservice endpoint: "
            << endpoint
            << '\n';

        std::cout
            << "\nChoose test:\n"
            << "1 - SendData success\n"
            << "2 - SendData NO_REPLY\n"
            << "3 - SendData TIMEOUT\n"
            << "4 - SendData gRPC driver error\n"
            << "5 - Subscribe\n"
            << "Choice: ";

        int choice = 0;
        std::cin >> choice;

        if (choice == 1)
        {
            const std::string data{
                static_cast<char>(0xAA),
                static_cast<char>(0xBB),
                static_cast<char>(0xCC)
            };

            testSendData(
                stub.get(),
                1,
                data
            );
        }
        else if (choice == 2)
        {
            testSendData(
                stub.get(),
                100,
                "test"
            );
        }
        else if (choice == 3)
        {
            testSendData(
                stub.get(),
                101,
                "test"
            );
        }
        else if (choice == 4)
        {
            testSendData(
                stub.get(),
                500,
                "test"
            );
        }
        else if (choice == 5)
        {
            testSubscribe(
                stub.get()
            );
        }
        else
        {
            std::cerr
                << "Unknown test choice\n";

            return 1;
        }

        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "Test client fatal error: "
            << exception.what()
            << '\n';

        return 1;
    }
    catch (...)
    {
        std::cerr
            << "Unknown test client fatal error\n";

        return 1;
    }
}
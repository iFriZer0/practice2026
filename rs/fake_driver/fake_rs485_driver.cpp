#include "fake_rs485_driver.h"

#include <chrono>
#include <cstdint>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <stdexcept>
#include <string>
#include <thread>

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

}

grpc::Status FakeRs485Driver::SendData(
    grpc::ServerContext* context,
    grpc::ServerReaderWriter<
        rs485::driver::v1::SendDataResponse,
        rs485::driver::v1::SendDataRequest>* stream)
{
    try
    {
        rs485::driver::v1::SendDataRequest request;

        while (!context->IsCancelled() &&
               stream->Read(&request))
        {
            std::cout
                << "\n[FAKE DRIVER] SendData request\n"
                << "channel_id: "
                << request.channel_id()
                << '\n'
                << "data: "
                << bytesToHex(request.data())
                << '\n';

            rs485::driver::v1::SendDataResponse response;

            response.set_channel_id(
                request.channel_id()
            );

            /*
             * Специальные тестовые значения channel_id:
             *
             * 100 -> NO_REPLY
             * 101 -> TIMEOUT
             * 102 -> WRONG_INTERFACE
             * 103 -> WRONG_PARAM
             * 500 -> gRPC INTERNAL
             */

            if (request.channel_id() == 500)
            {
                return grpc::Status(
                    grpc::StatusCode::INTERNAL,
                    "Simulated fake driver internal failure"
                );
            }

            if (request.channel_id() == 100)
            {
                response.set_success(false);
                response.set_error_message(
                    rs485::driver::v1::NO_REPLY
                );
            }
            else if (request.channel_id() == 101)
            {
                response.set_success(false);
                response.set_error_message(
                    rs485::driver::v1::TIMEOUT
                );
            }
            else if (request.channel_id() == 102)
            {
                response.set_success(false);
                response.set_error_message(
                    rs485::driver::v1::WRONG_INTERFACE
                );
            }
            else if (request.channel_id() == 103)
            {
                response.set_success(false);
                response.set_error_message(
                    rs485::driver::v1::WRONG_PARAM
                );
            }
            else
            {
                response.set_success(true);
                response.set_error_message(
                    rs485::driver::v1::NO_ERROR
                );
            }

            if (!stream->Write(response))
            {
                std::cout
                    << "[FAKE DRIVER] Client closed "
                    << "SendData stream\n";

                break;
            }
        }

        std::cout
            << "[FAKE DRIVER] SendData stream finished\n";

        return grpc::Status::OK;
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "[FAKE DRIVER] SendData exception: "
            << exception.what()
            << '\n';

        return grpc::Status(
            grpc::StatusCode::INTERNAL,
            exception.what()
        );
    }
    catch (...)
    {
        std::cerr
            << "[FAKE DRIVER] Unknown SendData exception\n";

        return grpc::Status(
            grpc::StatusCode::INTERNAL,
            "Unknown fake driver error"
        );
    }
}

grpc::Status FakeRs485Driver::Subscribe(
    grpc::ServerContext* context,
    const google::protobuf::Empty* request,
    grpc::ServerWriter<
        rs485::driver::v1::ReceiveDataResponse>* writer)
{
    try
    {
        static_cast<void>(request);

        std::cout
            << "\n[FAKE DRIVER] Subscribe started\n";

        std::uint32_t counter = 0;

        while (!context->IsCancelled())
        {
            rs485::driver::v1::ReceiveDataResponse response;

            response.set_success(true);

            response.set_channel_id(
                counter % 2 + 1
            );

            response.set_error_message(
                rs485::driver::v1::NO_ERROR
            );

            std::string packet;

            packet.push_back(
                static_cast<char>(counter & 0xFF)
            );

            packet.push_back(
                static_cast<char>((counter + 1) & 0xFF)
            );

            packet.push_back(
                static_cast<char>((counter + 2) & 0xFF)
            );

            response.add_data(packet);

            std::cout
                << "[FAKE DRIVER] Sending packet\n"
                << "channel_id: "
                << response.channel_id()
                << '\n'
                << "data: "
                << bytesToHex(packet)
                << '\n';

            if (!writer->Write(response))
            {
                std::cout
                    << "[FAKE DRIVER] Subscribe client "
                    << "closed connection\n";

                break;
            }

            ++counter;

            std::this_thread::sleep_for(
                std::chrono::seconds(1)
            );
        }

        std::cout
            << "[FAKE DRIVER] Subscribe finished\n";

        return grpc::Status::OK;
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "[FAKE DRIVER] Subscribe exception: "
            << exception.what()
            << '\n';

        return grpc::Status(
            grpc::StatusCode::INTERNAL,
            exception.what()
        );
    }
    catch (...)
    {
        std::cerr
            << "[FAKE DRIVER] Unknown Subscribe exception\n";

        return grpc::Status(
            grpc::StatusCode::INTERNAL,
            "Unknown fake driver error"
        );
    }
}
#include "fake_rs485_driver.h"

#include <grpcpp/grpcpp.h>

#include <exception>
#include <iostream>
#include <memory>
#include <string>

int main(int argc, char* argv[])
{
    try
    {
        std::string address = "127.0.0.1:50051";

        if (argc > 1)
        {
            address = argv[1];
        }

        FakeRs485Driver service;

        grpc::ServerBuilder builder;

        builder.AddListeningPort(
            address,
            grpc::InsecureServerCredentials()
        );

        builder.RegisterService(&service);

        std::unique_ptr<grpc::Server> server =
            builder.BuildAndStart();

        if (!server)
        {
            std::cerr
                << "Failed to start fake RS-485 driver at "
                << address
                << '\n';

            return 1;
        }

        std::cout
            << "Fake RS-485 driver started\n"
            << "Listening at: "
            << address
            << '\n';

        server->Wait();

        return 0;
    }
    catch (const std::exception& exception)
    {
        std::cerr
            << "Fake driver fatal error: "
            << exception.what()
            << '\n';

        return 1;
    }
    catch (...)
    {
        std::cerr
            << "Unknown fake driver fatal error\n";

        return 1;
    }
}
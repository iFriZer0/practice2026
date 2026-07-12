#include "rs485_driver_client.h"
#include "rs485_service.h"

#include <grpcpp/grpcpp.h>

#include <exception>
#include <iostream>
#include <memory>
#include <string>

int main(
    int argc,
    char *argv[])
{
    const std::string driver_endpoint =
        argc > 1
            ? argv[1]
            : "127.0.0.1:50051";

    const std::string service_endpoint =
        argc > 2
            ? argv[2]
            : "0.0.0.0:50052";

    try
    {
        auto driver_client =
            std::make_shared<Rs485DriverClient>();

        driver_client->connect(
            driver_endpoint
        );

        Rs485ServiceImpl service{
            driver_client
        };

        grpc::ServerBuilder builder;

        builder.AddListeningPort(
            service_endpoint,
            grpc::InsecureServerCredentials()
        );

        builder.RegisterService(
            &service
        );

        std::unique_ptr<grpc::Server> server =
            builder.BuildAndStart();

        if (!server)
        {
            std::cerr
                << "Failed to start the RS-485 microservice"
                << std::endl;

            return 1;
        }

        std::cout
            << "RS-485 microservice started"
            << std::endl;

        std::cout
            << "Driver endpoint: "
            << driver_endpoint
            << std::endl;

        std::cout
            << "Service endpoint: "
            << service_endpoint
            << std::endl;

        server->Wait();

        return 0;
    }
    catch (const std::exception &exception)
    {
        std::cerr
            << "RS-485 microservice error: "
            << exception.what()
            << std::endl;

        return 1;
    }
    catch (...)
    {
        std::cerr
            << "Unknown RS-485 microservice error"
            << std::endl;

        return 1;
    }
}
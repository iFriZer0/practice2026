#include "rs485_config.h"
#include "rs485_driver_client.h"
#include "rs485_service.h"

#include <grpcpp/grpcpp.h>

#include <cstdlib>
#include <exception>
#include <iostream>
#include <memory>
#include <string>

namespace
{

constexpr const char *DEFAULT_CONFIG_PATH =
    "config/config.yaml";

std::string resolveConfigPath()
{
    const char *const environment_path =
        std::getenv("RS485_CONFIG_PATH");

    if (environment_path != nullptr &&
        environment_path[0] != '\0')
    {
        return environment_path;
    }

    return DEFAULT_CONFIG_PATH;
}

} // namespace

int main()
{
    try
    {
        const std::string config_path =
            resolveConfigPath();

        const Rs485Config config =
            Rs485Config::load(config_path);

        const std::string driver_endpoint =
            config.driverAddress();

        const std::string service_endpoint =
            config.serviceAddress();

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
            throw std::runtime_error(
                "Failed to start the RS-485 microservice"
            );
        }

        std::cout
            << "RS-485 microservice started"
            << std::endl;

        std::cout
            << "Config path: "
            << config_path
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

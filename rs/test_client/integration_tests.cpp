#include <gtest/gtest.h>

#include <grpcpp/grpcpp.h>

#include "fake_rs485_driver.h"
#include "rs485_config.h"
#include "rs485_driver_client.h"
#include "rs485_service.h"
#include "rs485_service.grpc.pb.h"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <memory>
#include <string>

#ifndef RS485_TEST_CONFIG_PATH
#define RS485_TEST_CONFIG_PATH "config/config.yaml"
#endif

namespace
{

class Rs485IntegrationTest : public ::testing::Test
{
protected:
    void SetUp() override
    {
        disableProxyForLocalConnections();

        config_ =
            Rs485Config::load(
                RS485_TEST_CONFIG_PATH
            );

        startFakeDriver();
        connectDriverClient();
        startMicroservice();
        createServiceStub();
    }

    void TearDown() override
    {
        service_stub_.reset();
        service_channel_.reset();

        if (microservice_server_)
        {
            microservice_server_->Shutdown();
            microservice_server_->Wait();
            microservice_server_.reset();
        }

        service_.reset();

        if (driver_client_)
        {
            driver_client_->stopSubscribe();
            driver_client_.reset();
        }

        driver_probe_channel_.reset();

        if (fake_driver_server_)
        {
            fake_driver_server_->Shutdown();
            fake_driver_server_->Wait();
            fake_driver_server_.reset();
        }
    }

    rs485::service::v1::SendDataResponse sendData(
        std::uint32_t channel_id,
        const std::string &data)
    {
        grpc::ClientContext context;

        rs485::service::v1::SendDataRequest request;
        rs485::service::v1::SendDataResponse response;

        request.set_channel_id(channel_id);
        request.set_data(data);

        const grpc::Status status =
            service_stub_->SendData(
                &context,
                request,
                &response
            );

        EXPECT_TRUE(status.ok())
            << status.error_message();

        return response;
    }

    std::shared_ptr<grpc::Channel>
        service_channel_;

    std::unique_ptr<
        rs485::service::v1::Rs485Service::Stub
    > service_stub_;

private:
    static void disableProxyForLocalConnections()
    {
        setenv(
            "NO_PROXY",
            "127.0.0.1,localhost",
            1
        );

        setenv(
            "no_proxy",
            "127.0.0.1,localhost",
            1
        );

        unsetenv("grpc_proxy");
        unsetenv("GRPC_PROXY");
        unsetenv("http_proxy");
        unsetenv("HTTP_PROXY");
        unsetenv("https_proxy");
        unsetenv("HTTPS_PROXY");
        unsetenv("all_proxy");
        unsetenv("ALL_PROXY");
    }

    static void waitForChannel(
        const std::shared_ptr<grpc::Channel> &channel,
        const std::string &description)
    {
        ASSERT_NE(channel, nullptr);

        const bool connected =
            channel->WaitForConnected(
                std::chrono::system_clock::now() +
                std::chrono::seconds(5)
            );

        ASSERT_TRUE(connected)
            << "Failed to connect to "
            << description;
    }

    void startFakeDriver()
    {
        grpc::ServerBuilder builder;

        builder.AddListeningPort(
            config_.driverAddress(),
            grpc::InsecureServerCredentials()
        );

        builder.RegisterService(&fake_driver_);

        fake_driver_server_ =
            builder.BuildAndStart();

        ASSERT_NE(fake_driver_server_, nullptr)
            << "Failed to start fake driver at "
            << config_.driverAddress()
            << ". Make sure the port is not already occupied.";
    }

    void connectDriverClient()
    {
        driver_probe_channel_ =
            grpc::CreateChannel(
                config_.driverAddress(),
                grpc::InsecureChannelCredentials()
            );

        waitForChannel(
            driver_probe_channel_,
            "fake RS-485 driver at " +
                config_.driverAddress()
        );

        driver_client_ =
            std::make_shared<Rs485DriverClient>();

        ASSERT_TRUE(
            driver_client_->connect(
                config_.driverAddress()
            )
        );
    }

    void startMicroservice()
    {
        service_ =
            std::make_unique<Rs485ServiceImpl>(
                driver_client_
            );

        grpc::ServerBuilder builder;

        builder.AddListeningPort(
            config_.serviceAddress(),
            grpc::InsecureServerCredentials()
        );

        builder.RegisterService(service_.get());

        microservice_server_ =
            builder.BuildAndStart();

        ASSERT_NE(microservice_server_, nullptr)
            << "Failed to start RS-485 microservice at "
            << config_.serviceAddress()
            << ". Make sure the port is not already occupied.";
    }

    void createServiceStub()
    {
        service_channel_ =
            grpc::CreateChannel(
                config_.serviceAddress(),
                grpc::InsecureChannelCredentials()
            );

        waitForChannel(
            service_channel_,
            "RS-485 microservice at " +
                config_.serviceAddress()
        );

        service_stub_ =
            rs485::service::v1::Rs485Service::NewStub(
                service_channel_
            );

        ASSERT_NE(service_stub_, nullptr);
    }

    Rs485Config config_;

    FakeRs485Driver fake_driver_;

    std::unique_ptr<grpc::Server>
        fake_driver_server_;

    std::shared_ptr<grpc::Channel>
        driver_probe_channel_;

    std::shared_ptr<Rs485DriverClient>
        driver_client_;

    std::unique_ptr<Rs485ServiceImpl>
        service_;

    std::unique_ptr<grpc::Server>
        microservice_server_;
};

TEST_F(
    Rs485IntegrationTest,
    SendDataSuccess)
{
    const std::string data{
        static_cast<char>(0xAA),
        static_cast<char>(0xBB),
        static_cast<char>(0xCC)
    };

    const auto response =
        sendData(1, data);

    EXPECT_TRUE(response.success());
    EXPECT_EQ(response.channel_id(), 1U);
    EXPECT_EQ(
        response.error_code(),
        rs485::service::v1::NO_ERROR
    );
    EXPECT_TRUE(response.error_message().empty());
}

TEST_F(
    Rs485IntegrationTest,
    SendDataNoReply)
{
    const auto response =
        sendData(100, "test");

    EXPECT_FALSE(response.success());
    EXPECT_EQ(response.channel_id(), 100U);
    EXPECT_EQ(
        response.error_code(),
        rs485::service::v1::DRIVER_NO_REPLY
    );
    EXPECT_FALSE(response.error_message().empty());
}

TEST_F(
    Rs485IntegrationTest,
    SendDataTimeout)
{
    const auto response =
        sendData(101, "test");

    EXPECT_FALSE(response.success());
    EXPECT_EQ(response.channel_id(), 101U);
    EXPECT_EQ(
        response.error_code(),
        rs485::service::v1::DRIVER_TIMEOUT
    );
    EXPECT_FALSE(response.error_message().empty());
}

TEST_F(
    Rs485IntegrationTest,
    SendDataDriverGrpcError)
{
    const auto response =
        sendData(500, "test");

    EXPECT_FALSE(response.success());
    EXPECT_EQ(response.channel_id(), 500U);
    EXPECT_EQ(
        response.error_code(),
        rs485::service::v1::INTERNAL_ERROR
    );
    EXPECT_FALSE(response.error_message().empty());
}

TEST_F(
    Rs485IntegrationTest,
    SendDataRejectsEmptyData)
{
    const auto response =
        sendData(1, "");

    EXPECT_FALSE(response.success());
    EXPECT_EQ(response.channel_id(), 1U);
    EXPECT_EQ(
        response.error_code(),
        rs485::service::v1::INVALID_DATA
    );
    EXPECT_FALSE(response.error_message().empty());
}

TEST_F(
    Rs485IntegrationTest,
    SubscribeReceivesPacket)
{
    grpc::ClientContext context;

    context.set_deadline(
        std::chrono::system_clock::now() +
        std::chrono::seconds(5)
    );

    rs485::service::v1::SubscribeRequest request;
    rs485::service::v1::ReceiveDataResponse response;

    std::unique_ptr<
        grpc::ClientReader<
            rs485::service::v1::ReceiveDataResponse
        >
    > reader =
        service_stub_->Subscribe(
            &context,
            request
        );

    ASSERT_NE(reader, nullptr);
    ASSERT_TRUE(reader->Read(&response));

    EXPECT_TRUE(response.success());
    EXPECT_EQ(response.channel_id(), 1U);
    EXPECT_EQ(
        response.error_code(),
        rs485::service::v1::NO_ERROR
    );

    const std::string expected_data{
        static_cast<char>(0x00),
        static_cast<char>(0x01),
        static_cast<char>(0x02)
    };

    EXPECT_EQ(response.data(), expected_data);

    context.TryCancel();

    const grpc::Status status =
        reader->Finish();

    EXPECT_TRUE(
        status.ok() ||
        status.error_code() ==
            grpc::StatusCode::CANCELLED
    );
}

} // namespace
#ifndef PKU_DRIVER_PLUG_H__

#define PKU_DRIVER_PLUG_H__

#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <google/protobuf/empty.pb.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include "main_information.h"
#include "pku_driver.grpc.pb.h"
#include "pku_driver.pb.h"

class PkuDriverPlug : public pku::driver::v1::PkuDriver::Service
{
public:
    explicit PkuDriverPlug();
    explicit PkuDriverPlug(const PkuDriverPlug &other);
    explicit PkuDriverPlug(PkuDriverPlug &&other) noexcept;

    ~PkuDriverPlug() = default;

    PkuDriverPlug &operator=(const PkuDriverPlug &other);
    PkuDriverPlug &operator=(PkuDriverPlug &&other);

    grpc::Status CheckConnection(
            grpc::ServerContext *context,
            const google::protobuf::Empty *request,
            pku::driver::v1::StandardResponse *response
    ) override;

    grpc::Status GetHardwareStatus(
            grpc::ServerContext *context,
            const google::protobuf::Empty *request,
            pku::driver::v1::StandardResponse *response
    ) override;

    grpc::Status ReadMainInfo(
            grpc::ServerContext *context,
            const google::protobuf::Empty *request,
            pku::driver::v1::ReadMemoryResponse *response
    ) override;

    grpc::Status WriteMainInfo(
            grpc::ServerContext *context,
            const pku::driver::v1::WriteMainInfoRequest *request,
            pku::driver::v1::StandardResponse *response
    ) override;

    grpc::Status ReadPku(
            grpc::ServerContext *context,
            const pku::driver::v1::ReadPkuRequest *request,
            pku::driver::v1::ReadPkuResponse *response
    ) override;

    grpc::Status SetPkuMode(
            grpc::ServerContext *context,
            const pku::driver::v1::SetPkuModeRequest *request,
            pku::driver::v1::StandardResponse *response
    ) override;

    grpc::Status SendRkByIndex(
            grpc::ServerContext *context,
            const pku::driver::v1::SendRkByIndexRequest *request,
            pku::driver::v1::StandardResponse *response
    ) override;

    grpc::Status GetVersion(
            grpc::ServerContext *context,
            const google::protobuf::Empty *request,
            pku::driver::v1::VersionInfo *response
    ) override;
private:
    static constexpr std::size_t MAC_SIZE{6};
    static constexpr std::size_t ADDRESS_SIZE{4};

    static constexpr int HEXADECIMAL{16};

    static constexpr std::size_t PKU_COUNT{64};
    static constexpr std::uint32_t INITIAL_PKU_DURATION{1};
    static constexpr std::uint32_t INITIAL_MODE{0};

    static constexpr std::size_t RK_COUNT{48};
    static constexpr std::uint32_t INITIAL_RK_DURATION{1};

    MainInformation main_information;

    std::vector<std::uint32_t> pku_durations_ms;
    std::vector<std::uint32_t> pku_modes;

    std::vector<std::uint32_t> rk_durations_ms;

    std::string convert_main_information() const;

    std::string convert_buffer_size(const std::uint32_t &buffer_size) const;

    std::string convert_mac(const std::string &mac) const;

    std::string convert_address(const std::string &address) const;
};

#endif

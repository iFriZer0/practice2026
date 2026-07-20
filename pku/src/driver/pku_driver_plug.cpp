#include <algorithm>
#include <array>
#include <bit>
#include <charconv>
#include <cstdint>
#include <iterator>
#include <ranges>
#include <string>
#include <sstream>
#include <string_view>
#include <utility>
#include <google/protobuf/empty.pb.h>
#include <google/protobuf/repeated_field.h>
#include <grpcpp/server_context.h>
#include <grpcpp/support/status.h>
#include "pku_driver_plug.h"
#include "pku_driver.pb.h"

PkuDriverPlug::PkuDriverPlug()
    : pku_durations_ms(PKU_COUNT, INITIAL_PKU_DURATION),
      pku_modes(PKU_COUNT, INITIAL_MODE),
      rk_durations_ms{RK_COUNT, INITIAL_RK_DURATION} {}

PkuDriverPlug::PkuDriverPlug(const PkuDriverPlug &other)
    : main_information{other.main_information},
      pku_durations_ms{other.pku_durations_ms},
      pku_modes{other.pku_modes},
      rk_durations_ms{other.rk_durations_ms} {}

PkuDriverPlug::PkuDriverPlug(PkuDriverPlug &&other) noexcept
    : main_information{std::move(other.main_information)},
      pku_durations_ms{std::move(other.pku_durations_ms)},
      pku_modes{std::move(other.pku_modes)},
      rk_durations_ms{std::move(other.rk_durations_ms)} {}

PkuDriverPlug &PkuDriverPlug::operator=(const PkuDriverPlug &other)
{
    main_information = other.main_information;
    pku_durations_ms = other.pku_durations_ms;
    pku_modes = other.pku_modes;
    rk_durations_ms = other.rk_durations_ms;
    return *this;
}

PkuDriverPlug &PkuDriverPlug::operator=(PkuDriverPlug &&other)
{
    main_information = std::move(other.main_information);
    pku_durations_ms = std::move(other.pku_durations_ms);
    pku_modes = std::move(other.pku_modes);
    rk_durations_ms = std::move(other.rk_durations_ms);
    return *this;
}

grpc::Status PkuDriverPlug::CheckConnection(
        grpc::ServerContext *context,
        const google::protobuf::Empty *request,
        pku::driver::v1::StandardResponse *response
)
{
    response->set_success(true);
    response->set_error_message("");
    return grpc::Status::OK;
}

grpc::Status PkuDriverPlug::GetHardwareStatus(
        grpc::ServerContext *context,
        const google::protobuf::Empty *request,
        pku::driver::v1::StandardResponse *response
)
{
    response->set_success(true);
    response->set_error_message("");
    return grpc::Status::OK;
}

grpc::Status PkuDriverPlug::ReadMainInfo(
        grpc::ServerContext *context,
        const google::protobuf::Empty *request,
        pku::driver::v1::ReadMemoryResponse *response
)
{
    response->set_success(true);
    response->set_error_message("");
    response->set_data(convert_main_information());
    return grpc::Status::OK;
}

grpc::Status PkuDriverPlug::WriteMainInfo(
        grpc::ServerContext *context,
        const pku::driver::v1::WriteMainInfoRequest *request,
        pku::driver::v1::StandardResponse *response
)
{
    main_information.description.assign(convert_description(request->description()), 0, DESCRIPTION_SIZE);
    main_information.mac.assign(convert_mac(request->mac()), 0, MAC_SIZE);
    main_information.ip.assign(convert_address(request->ip()), 0, ADDRESS_SIZE);
    main_information.netmask.assign(convert_address(request->netmask()), 0, ADDRESS_SIZE);
    main_information.gateway.assign(convert_address(request->gateway()), 0, ADDRESS_SIZE);
    main_information.dns.assign(convert_address(request->dns()), 0, ADDRESS_SIZE);
    main_information.use_dhcp = request->use_dhcp();
    response->set_success(true);
    response->set_error_message("");
    return grpc::Status::OK;
}

grpc::Status PkuDriverPlug::ReadPku(
        grpc::ServerContext *context,
        const pku::driver::v1::ReadPkuRequest *request,
        pku::driver::v1::ReadPkuResponse *response
)
{
    response->set_success(true);
    response->set_error_message("");
    response->mutable_durations_ms()->Resize(PKU_COUNT, 0);
    std::ranges::for_each(request->indices(), [this, response](const std::uint32_t &index)
    {
        response->set_durations_ms(index - 1, pku_durations_ms[index - 1]);
    });
    return grpc::Status::OK;
}

grpc::Status PkuDriverPlug::SetPkuMode(
        grpc::ServerContext *context,
        const pku::driver::v1::SetPkuModeRequest *request,
        pku::driver::v1::StandardResponse *response
)
{
    response->set_success(true);
    response->set_error_message("");
    pku_modes[request->pku_index() - 1] = request->mode();
    return grpc::Status::OK;
}

grpc::Status PkuDriverPlug::SendRkByIndex(
        grpc::ServerContext *context,
        const pku::driver::v1::SendRkByIndexRequest *request,
        pku::driver::v1::StandardResponse *response
)
{
    response->set_success(true);
    response->set_error_message("");
    rk_durations_ms[request->rk_index() - 1] = request->duration_ms();
    return grpc::Status::OK;
}

grpc::Status PkuDriverPlug::GetVersion(
        grpc::ServerContext *context,
        const google::protobuf::Empty *request,
        pku::driver::v1::VersionInfo *response
)
{
    response->set_version(VERSION);
    return grpc::Status::OK;
}

const std::string PkuDriverPlug::VERSION{"0.04"};

std::string PkuDriverPlug::convert_main_information() const
{
    std::ostringstream stream;
    stream
            << main_information.identifier
            << convert_buffer_size(main_information.buffer_size)
            << main_information.description
            << main_information.mac
            << main_information.ip
            << main_information.netmask
            << main_information.gateway
            << main_information.dns
            << static_cast<char>(main_information.use_dhcp);
    return stream.str();
}

std::string PkuDriverPlug::convert_buffer_size(const std::uint32_t &buffer_size) const
{
    std::array<char, sizeof(std::uint32_t)> bits{std::bit_cast<std::array<char, sizeof(std::uint32_t)>>(buffer_size)};
    return std::string{bits.begin(), bits.end()};
}

std::string PkuDriverPlug::convert_description(const std::string &description) const
{
    std::string result{description, 0, DESCRIPTION_SIZE};
    result.resize(DESCRIPTION_SIZE);
    return result;
}

std::string PkuDriverPlug::convert_mac(const std::string &mac) const
{
    std::string result;
    result.reserve(MAC_SIZE);
    std::ranges::transform(mac | std::views::split(':'), std::back_inserter(result), [](auto &&byte)
    {
        std::string_view byte_view(&*byte.begin(), std::ranges::distance(byte));
        unsigned char value;
        std::from_chars(byte_view.data(), byte_view.data() + byte_view.size(), value, HEXADECIMAL);
        return static_cast<char>(value);
    });
    return result;
}

std::string PkuDriverPlug::convert_address(const std::string &address) const
{
    std::string result;
    result.reserve(ADDRESS_SIZE);
    std::ranges::transform(address | std::views::split('.'), std::back_inserter(result), [](auto &&byte)
    {
        std::string_view byte_view(&*byte.begin(), std::ranges::distance(byte));
        unsigned char value;
        std::from_chars(byte_view.data(), byte_view.data() + byte_view.size(), value);
        return static_cast<char>(value);
    });
    return result;
}

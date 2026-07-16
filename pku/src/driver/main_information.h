#ifndef MAIN_INFORMATION_H__

#define MAIN_INFORMATION_H__

#include <cstddef>
#include <cstdint>
#include <string>

struct MainInformation
{
public:
    std::string identifier{"\x50\x4B\x55\x69"};
    std::uint32_t buffer_size{1500};
    std::string description{INITIAL_DESCRIPTION + std::string(DESCRIPTION_SIZE - INITIAL_DESCRIPTION.size(), '\0')};
    std::string mac{"\x00\x08\xDC\x01\xAB\xD0", MAC_SIZE};
    std::string ip{static_cast<char>(192), static_cast<char>(168), static_cast<char>(2), static_cast<char>(251)};
    std::string netmask{static_cast<char>(255), static_cast<char>(255), static_cast<char>(0), static_cast<char>(0)};
    std::string gateway{static_cast<char>(192), static_cast<char>(168), static_cast<char>(1), static_cast<char>(1)};
    std::string dns{static_cast<char>(8), static_cast<char>(8), static_cast<char>(8), static_cast<char>(8)};
    bool use_dhcp{true};
private:
    static const std::string INITIAL_DESCRIPTION;
    static constexpr std::size_t DESCRIPTION_SIZE{120};
    static constexpr std::size_t MAC_SIZE{6};
};

#endif

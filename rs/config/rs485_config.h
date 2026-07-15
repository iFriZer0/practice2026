#ifndef RS485_CONFIG_H__
#define RS485_CONFIG_H__

#include <cstdint>
#include <string>

class Rs485Config
{
public:
    struct Address
    {
        std::string host;
        std::uint16_t port{0};

        std::string toString() const;
    };

    static Rs485Config load(
        const std::string &config_path
    );

    const Address &service() const noexcept;
    const Address &driver() const noexcept;

    std::string serviceAddress() const;
    std::string driverAddress() const;

private:
    Address service_;
    Address driver_;
};

#endif

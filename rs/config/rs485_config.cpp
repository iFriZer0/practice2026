#include "rs485_config.h"

#include <limits>
#include <stdexcept>
#include <string>

#include <yaml-cpp/yaml.h>

namespace
{

Rs485Config::Address readAddress(
    const YAML::Node &root,
    const std::string &section_name,
    const std::string &config_path)
{
    const YAML::Node section = root[section_name];

    if (!section || !section.IsMap())
    {
        throw std::runtime_error(
            "Missing or invalid '" + section_name +
            "' section in config: " + config_path
        );
    }

    const YAML::Node host_node = section["host"];
    const YAML::Node port_node = section["port"];

    if (!host_node || !host_node.IsScalar())
    {
        throw std::runtime_error(
            "Missing or invalid '" + section_name +
            ".host' in config: " + config_path
        );
    }

    if (!port_node || !port_node.IsScalar())
    {
        throw std::runtime_error(
            "Missing or invalid '" + section_name +
            ".port' in config: " + config_path
        );
    }

    Rs485Config::Address address;

    try
    {
        address.host = host_node.as<std::string>();

        const int port = port_node.as<int>();

        if (port <= 0 ||
            port > std::numeric_limits<std::uint16_t>::max())
        {
            throw std::out_of_range(
                "port must be in range 1..65535"
            );
        }

        address.port = static_cast<std::uint16_t>(port);
    }
    catch (const std::exception &exception)
    {
        throw std::runtime_error(
            "Invalid address in section '" + section_name +
            "' of config " + config_path + ": " +
            exception.what()
        );
    }

    if (address.host.empty())
    {
        throw std::runtime_error(
            "Empty '" + section_name +
            ".host' in config: " + config_path
        );
    }

    return address;
}

} // namespace

std::string Rs485Config::Address::toString() const
{
    return host + ":" + std::to_string(port);
}

Rs485Config Rs485Config::load(
    const std::string &config_path)
{
    if (config_path.empty())
    {
        throw std::invalid_argument(
            "The RS-485 config path is empty"
        );
    }

    YAML::Node root;

    try
    {
        root = YAML::LoadFile(config_path);
    }
    catch (const YAML::Exception &exception)
    {
        throw std::runtime_error(
            "Failed to load RS-485 config '" +
            config_path + "': " + exception.what()
        );
    }

    if (!root || !root.IsMap())
    {
        throw std::runtime_error(
            "The RS-485 config root must be a map: " +
            config_path
        );
    }

    Rs485Config config;
    config.service_ = readAddress(
        root,
        "service",
        config_path
    );
    config.driver_ = readAddress(
        root,
        "driver",
        config_path
    );

    return config;
}

const Rs485Config::Address &
Rs485Config::service() const noexcept
{
    return service_;
}

const Rs485Config::Address &
Rs485Config::driver() const noexcept
{
    return driver_;
}

std::string Rs485Config::serviceAddress() const
{
    return service_.toString();
}

std::string Rs485Config::driverAddress() const
{
    return driver_.toString();
}

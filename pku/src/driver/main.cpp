#include <fstream>
#include <memory>
#include <string>
#include <regex>
#include <grpcpp/grpcpp.h>
#include <grpcpp/security/server_credentials.h>
#include <grpcpp/server.h>
#include "pku_driver_plug.h"

const std::string IP{"0.0.0.0"};

static std::string extract_value(const std::string &line, const std::string &key)
{
    std::regex pattern("\"" + key + "\"\\s*:\\s*\"([^\"]*)\"");
    std::smatch match;
    std::string result;
    if (std::regex_search(line, match, pattern) && match.size() > 1)
    {
        result = match[1].str();
    }
    return result;
}

static std::string parse_port(const std::string &path)
{
    std::ifstream file{path};
    std::string line;
    std::string port;
    while (std::getline(file, line) && port.empty())
    {
        if (port.empty())
        {
            port = extract_value(line, "port");
        }
    }
    return port;
}

int main()
{
    grpc::ServerBuilder builder;
    builder.AddListeningPort(IP + ":" + parse_port("../configuration/driver_address.json"), grpc::InsecureServerCredentials());
    PkuDriverPlug driver;
    builder.RegisterService(&driver);
    std::unique_ptr<grpc::Server> server{builder.BuildAndStart()};
    server->Wait();
    return 0;
}

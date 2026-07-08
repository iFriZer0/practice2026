#ifndef MKO_CLIENT_H__
#define MKO_CLIENT_H__

#include <cstdint>
#include <string>
#include <vector>

struct ConfigureKKRequestData
{
    std::string board_id;
    int32_t index;
    int32_t channel;
    int32_t bus_control;
    int32_t ou_address;
    int32_t ou_resp_word;
    int32_t vector_word;
    int32_t selftest_word;
    std::string remote_ip;
    int32_t remote_port;
    std::string operation_id;
};

struct ConfigureExchangeRequestData
{
    std::string board_id;
    int32_t format;
    uint32_t ks1;
    uint32_t ks2;
    std::vector<uint32_t> sd;
    std::string operation_id;
};

struct RunExchangeRequestData
{
    std::string board_id;
    std::string operation_id;
};

struct ExchangeResultData
{
    int32_t format;
    uint32_t ks1;
    uint32_t ks2;
    std::vector<uint32_t> sd;
    uint32_t answer_word_1;
    uint32_t answer_word_2;
    uint32_t result_word;
    std::string decoded_result;
};

class MkoClient
{
public:
    virtual ~MkoClient() = default;

    virtual void configure_kk(const ConfigureKKRequestData &request) = 0;
    virtual void configure_exchange(const ConfigureExchangeRequestData &request) = 0;
    virtual void run_exchange(const RunExchangeRequestData &request) = 0;
};

#endif
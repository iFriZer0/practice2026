#ifndef MKO_CLIENT_H__
#define MKO_CLIENT_H__

#include <cstdint>
#include <functional>
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

struct ConfigureOuRequestData
{
    std::string board_id;
    int32_t index;
    int32_t channel;
    int32_t ou_address;
    int32_t response_word;
    std::string remote_ip;
    int32_t remote_port;
};

struct SetOuResponseWordRequestData
{
    std::string board_id;
    int32_t index;
    int32_t channel;
    int32_t ou_address;
    int32_t response_word;
};

struct ReadOuSubaddressRequestData
{
    std::string board_id;
    uint32_t subaddress;
    bool receive_area;
};

struct OuSubaddressData
{
    uint32_t subaddress;
    std::vector<uint32_t> sd;
    uint32_t cmd_word;
    uint32_t result_word;
    std::string decoded_result;
};

struct WriteOuSubaddressRequestData
{
    std::string board_id;
    uint32_t subaddress;
    std::vector<uint32_t> sd;
};

struct ClearBufferRequestData
{
    std::string board_id;
    int32_t index;
};

struct SubscribeOuCommandsRequestData
{
    std::string board_id;
};

// OuCommandEventData — одно событие входящей команды ОУ из потока
// SubscribeOuCommands. Поля совпадают с OuCommandEvent из mko.proto.
struct OuCommandEventData
{
    uint32_t cmd_word;
    uint32_t result_word;
    bool receive_from_ou;
    uint32_t ou_address;
    uint32_t subaddress;
    uint32_t word_count;
    std::string decoded_command;
    std::string decoded_result;
};

class MkoClient
{
public:
    // Обработчик одного события ОУ, вызывается из фонового потока,
    // читающего server-streaming RPC SubscribeOuCommands. Вызывающая
    // сторона (GUI) сама отвечает за то, чтобы попасть в поток GUI,
    // прежде чем трогать виджеты (см. использование в qt_view_mko.cpp
    // через QMetaObject::invokeMethod).
    using OuCommandEventHandler = std::function<void(const OuCommandEventData &)>;

    // Обработчик завершения потока с ошибкой (сервер закрыл поток не
    // штатно). Не вызывается при штатной отписке через
    // unsubscribe_ou_commands().
    using OuSubscriptionErrorHandler = std::function<void(const std::string &)>;

    virtual ~MkoClient() = default;

    virtual void configure_kk(const ConfigureKKRequestData &request) = 0;
    virtual void configure_exchange(const ConfigureExchangeRequestData &request) = 0;
    virtual void run_exchange(const RunExchangeRequestData &request) = 0;
    virtual void configure_ou(const ConfigureOuRequestData &request) = 0;
    virtual void set_ou_response_word(const SetOuResponseWordRequestData &request) = 0;
    virtual OuSubaddressData read_ou_subaddress(const ReadOuSubaddressRequestData &request) = 0;
    virtual void write_ou_subaddress(const WriteOuSubaddressRequestData &request) = 0;
    virtual void send_raw_ou_data(const WriteOuSubaddressRequestData &request) = 0;
    virtual void clear_receive_buffer(const ClearBufferRequestData &request) = 0;
    virtual void clear_transmit_buffer(const ClearBufferRequestData &request) = 0;

    // Открывает server-streaming RPC SubscribeOuCommands в фоновом
    // потоке и вызывает on_event на каждое входящее событие, пока
    // поток не будет закрыт сервером или явно остановлен через
    // unsubscribe_ou_commands(). Повторный вызов сначала останавливает
    // предыдущую подписку.
    virtual void subscribe_ou_commands(const SubscribeOuCommandsRequestData &request,
                                        OuCommandEventHandler on_event,
                                        OuSubscriptionErrorHandler on_error) = 0;

    // Останавливает текущую подписку (если есть) и дожидается
    // завершения фонового потока. Безопасно вызывать, даже если
    // подписки не было.
    virtual void unsubscribe_ou_commands() = 0;
};

#endif

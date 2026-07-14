#include "rs485_service.h"

#include "rs485_driver_client.h"
#include "rs485_errors.h"

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <deque>
#include <exception>
#include <mutex>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

namespace
{

void fillErrorResponse(
    rs485::service::v1::SendDataResponse *response,
    std::uint32_t channel_id,
    rs485::service::v1::ErrorCode error_code,
    const std::string &error_message)
{
    response->set_success(false);
    response->set_channel_id(channel_id);
    response->set_error_code(error_code);
    response->set_error_message(error_message);
}

rs485::service::v1::ErrorCode
serviceCodeFromDriverError(
    rs485::driver::v1::PultErrors error)
{
    using DriverError =
        rs485::driver::v1::PultErrors;

    using ServiceError =
        rs485::service::v1::ErrorCode;

    if (error == DriverError::NO_ERROR)
    {
        return ServiceError::NO_ERROR;
    }

    if (error == DriverError::NO_REPLY)
    {
        return ServiceError::DRIVER_NO_REPLY;
    }

    if (error == DriverError::TIMEOUT)
    {
        return ServiceError::DRIVER_TIMEOUT;
    }

    return ServiceError::DRIVER_ERROR;
}

rs485::service::v1::ErrorCode
serviceCodeFromGrpcError(
    grpc::StatusCode status_code)
{
    using ServiceError =
        rs485::service::v1::ErrorCode;

    if (status_code ==
        grpc::StatusCode::UNAVAILABLE)
    {
        return ServiceError::DRIVER_NOT_CONNECTED;
    }

    if (status_code ==
        grpc::StatusCode::DEADLINE_EXCEEDED)
    {
        return ServiceError::DRIVER_TIMEOUT;
    }

    if (status_code ==
        grpc::StatusCode::INTERNAL)
    {
        return ServiceError::INTERNAL_ERROR;
    }

    return ServiceError::DRIVER_ERROR;
}

} // namespace

Rs485ServiceImpl::Rs485ServiceImpl(
    std::shared_ptr<Rs485DriverClient> driver_client)
    : driver_client_{
          std::move(driver_client)
      }
{
    if (!driver_client_)
    {
        throw std::invalid_argument(
            "The RS-485 driver client is null"
        );
    }
}

grpc::Status Rs485ServiceImpl::SendData(
    grpc::ServerContext *context,
    const rs485::service::v1::SendDataRequest *request,
    rs485::service::v1::SendDataResponse *response)
{
    static_cast<void>(context);

    if (request == nullptr ||
        response == nullptr)
    {
        return grpc::Status{
            grpc::StatusCode::INTERNAL,
            "Invalid SendData request objects"
        };
    }

    const std::uint32_t channel_id =
        request->channel_id();

    try
    {
        if (request->data().empty())
        {
            throw Rs485ValidationException(
                "The data field is empty"
            );
        }

        const std::string &raw_data =
            request->data();

        const std::vector<std::uint8_t> bytes(
            raw_data.begin(),
            raw_data.end()
        );

        const SendDataResult result =
            driver_client_->sendData(
                channel_id,
                bytes
            );

        if (!result.success)
        {
            fillErrorResponse(
                response,
                result.channel_id,
                rs485::service::v1::DRIVER_ERROR,
                result.error_message
            );

            return grpc::Status::OK;
        }

        response->set_success(true);
        response->set_channel_id(
            result.channel_id
        );
        response->set_error_code(
            rs485::service::v1::NO_ERROR
        );
        response->clear_error_message();

        return grpc::Status::OK;
    }
    catch (const Rs485ValidationException &exception)
    {
        fillErrorResponse(
            response,
            channel_id,
            rs485::service::v1::INVALID_DATA,
            exception.what()
        );
    }
    catch (const Rs485ConnectionException &exception)
    {
        fillErrorResponse(
            response,
            channel_id,
            rs485::service::v1::DRIVER_NOT_CONNECTED,
            exception.what()
        );
    }
    catch (const Rs485DriverException &exception)
    {
        fillErrorResponse(
            response,
            channel_id,
            serviceCodeFromDriverError(
                exception.errorCode()
            ),
            exception.what()
        );
    }
    catch (const Rs485GrpcException &exception)
    {
        fillErrorResponse(
            response,
            channel_id,
            serviceCodeFromGrpcError(
                exception.statusCode()
            ),
            exception.what()
        );
    }
    catch (const Rs485StreamException &exception)
    {
        fillErrorResponse(
            response,
            channel_id,
            rs485::service::v1::DRIVER_ERROR,
            exception.what()
        );
    }
    catch (const std::exception &exception)
    {
        fillErrorResponse(
            response,
            channel_id,
            rs485::service::v1::INTERNAL_ERROR,
            exception.what()
        );
    }
    catch (...)
    {
        fillErrorResponse(
            response,
            channel_id,
            rs485::service::v1::INTERNAL_ERROR,
            "Unknown internal RS-485 service error"
        );
    }

    return grpc::Status::OK;
}

grpc::Status Rs485ServiceImpl::Subscribe(
    grpc::ServerContext *context,
    const rs485::service::v1::SubscribeRequest *request,
    grpc::ServerWriter<
        rs485::service::v1::ReceiveDataResponse
    > *writer)
{
    static_cast<void>(request);

    if (context == nullptr ||
        writer == nullptr)
    {
        return grpc::Status{
            grpc::StatusCode::INTERNAL,
            "Invalid Subscribe request objects"
        };
    }

    std::mutex queue_mutex;
    std::condition_variable queue_condition;
    std::deque<ReceiveDataResult> result_queue;

    bool accepting_results = true;

    const auto callback =
        [&queue_mutex,
         &queue_condition,
         &result_queue,
         &accepting_results](
            const ReceiveDataResult &result)
        {
            {
                std::lock_guard<std::mutex> lock{
                    queue_mutex
                };

                if (!accepting_results)
                {
                    return;
                }

                result_queue.push_back(result);
            }

            queue_condition.notify_one();
        };

    try
    {
        driver_client_->startSubscribe(
            callback
        );
    }
    catch (const Rs485ConnectionException &exception)
    {
        return grpc::Status{
            grpc::StatusCode::UNAVAILABLE,
            exception.what()
        };
    }
    catch (const Rs485ValidationException &exception)
    {
        return grpc::Status{
            grpc::StatusCode::INVALID_ARGUMENT,
            exception.what()
        };
    }
    catch (const Rs485GrpcException &exception)
    {
        return grpc::Status{
            exception.statusCode(),
            exception.what()
        };
    }
    catch (const Rs485Exception &exception)
    {
        return grpc::Status{
            grpc::StatusCode::INTERNAL,
            exception.what()
        };
    }
    catch (const std::exception &exception)
    {
        return grpc::Status{
            grpc::StatusCode::INTERNAL,
            exception.what()
        };
    }
    catch (...)
    {
        return grpc::Status{
            grpc::StatusCode::INTERNAL,
            "Unknown RS-485 subscription error"
        };
    }

    grpc::Status result_status =
        grpc::Status::OK;

    try
    {
        while (!context->IsCancelled())
        {
            ReceiveDataResult result;

            {
                std::unique_lock<std::mutex> lock{
                    queue_mutex
                };

                queue_condition.wait_for(
                    lock,
                    std::chrono::milliseconds{100},
                    [&result_queue, context]()
                    {
                        return
                            !result_queue.empty() ||
                            context->IsCancelled();
                    }
                );

                if (context->IsCancelled())
                {
                    break;
                }

                if (result_queue.empty())
                {
                    continue;
                }

                result = std::move(
                    result_queue.front()
                );

                result_queue.pop_front();
            }

            rs485::service::v1::ReceiveDataResponse
                response;

            response.set_success(
                result.success
            );

            response.set_channel_id(
                result.channel_id
            );

            std::string raw_data;

            for (const ReceiveDataPacket &packet :
                 result.packets)
            {
                raw_data.append(
                    reinterpret_cast<const char *>(
                        packet.bytes.data()
                    ),
                    packet.bytes.size()
                );
            }

            response.set_data(
                raw_data
            );

            if (result.success)
            {
                response.set_error_code(
                    rs485::service::v1::NO_ERROR
                );

                response.clear_error_message();
            }
            else
            {
                response.set_error_code(
                    rs485::service::v1::DRIVER_ERROR
                );

                response.set_error_message(
                    result.error_message
                );
            }

            if (!writer->Write(response))
            {
                break;
            }
            if (response.error_code() ==
                rs485::service::v1::DRIVER_NOT_CONNECTED)
            {
                break;
            }
        }
    }
    catch (const std::exception &exception)
    {
        result_status = grpc::Status{
            grpc::StatusCode::INTERNAL,
            exception.what()
        };
    }
    catch (...)
    {
        result_status = grpc::Status{
            grpc::StatusCode::INTERNAL,
            "Unknown error while sending RS-485 data"
        };
    }

    {
        std::lock_guard<std::mutex> lock{
            queue_mutex
        };

        accepting_results = false;
    }

    queue_condition.notify_all();

    try
    {
        driver_client_->stopSubscribe();
    }
    catch (const std::exception &exception)
    {
        if (result_status.ok() &&
            !context->IsCancelled())
        {
            result_status = grpc::Status{
                grpc::StatusCode::INTERNAL,
                exception.what()
            };
        }
    }
    catch (...)
    {
        if (result_status.ok() &&
            !context->IsCancelled())
        {
            result_status = grpc::Status{
                grpc::StatusCode::INTERNAL,
                "Failed to stop RS-485 subscription"
            };
        }
    }

    return result_status;
}

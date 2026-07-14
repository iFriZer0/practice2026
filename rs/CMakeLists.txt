cmake_minimum_required(VERSION 3.16)

project(
    rs485_microservice
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

find_package(Protobuf REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads REQUIRED)

find_program(
    PROTOC_EXECUTABLE
    protoc
    REQUIRED
)

find_program(
    GRPC_CPP_PLUGIN
    grpc_cpp_plugin
    REQUIRED
)

set(
    RS485_API_DIR
    "${CMAKE_CURRENT_SOURCE_DIR}/api"
)

set(
    RS485_GENERATED_DIR
    "${CMAKE_CURRENT_BINARY_DIR}/generated"
)

file(
    MAKE_DIRECTORY
    "${RS485_GENERATED_DIR}"
)

set(
    RS485_DRIVER_PROTO
    "${RS485_API_DIR}/rs485_driver.proto"
)

set(
    RS485_SERVICE_PROTO
    "${RS485_API_DIR}/rs485_service.proto"
)

set(
    RS485_GENERATED_SOURCES
    "${RS485_GENERATED_DIR}/rs485_driver.pb.cc"
    "${RS485_GENERATED_DIR}/rs485_driver.grpc.pb.cc"
    "${RS485_GENERATED_DIR}/rs485_service.pb.cc"
    "${RS485_GENERATED_DIR}/rs485_service.grpc.pb.cc"
)

set(
    RS485_GENERATED_HEADERS
    "${RS485_GENERATED_DIR}/rs485_driver.pb.h"
    "${RS485_GENERATED_DIR}/rs485_driver.grpc.pb.h"
    "${RS485_GENERATED_DIR}/rs485_service.pb.h"
    "${RS485_GENERATED_DIR}/rs485_service.grpc.pb.h"
)

add_custom_command(
    OUTPUT
        ${RS485_GENERATED_SOURCES}
        ${RS485_GENERATED_HEADERS}

    COMMAND
        "${PROTOC_EXECUTABLE}"

    ARGS
        "--proto_path=${RS485_API_DIR}"
        "--cpp_out=${RS485_GENERATED_DIR}"
        "--grpc_out=${RS485_GENERATED_DIR}"
        "--plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN}"
        "${RS485_DRIVER_PROTO}"
        "${RS485_SERVICE_PROTO}"

    DEPENDS
        "${RS485_DRIVER_PROTO}"
        "${RS485_SERVICE_PROTO}"

    VERBATIM
)

add_custom_target(
    rs485_generate_proto
    DEPENDS
        ${RS485_GENERATED_SOURCES}
        ${RS485_GENERATED_HEADERS}
)

add_executable(
    rs485_microservice

    client/rs485_driver_client.cpp
    client/rs485_subscriber.cpp

    common/rs485_errors.cpp
    common/rs485_utils.cpp
    common/rs485_file_reader.cpp

    service/rs485_service.cpp
    service/main.cpp

    ${RS485_GENERATED_SOURCES}
)

add_dependencies(
    rs485_microservice
    rs485_generate_proto
)

target_include_directories(
    rs485_microservice
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/client"
        "${CMAKE_CURRENT_SOURCE_DIR}/common"
        "${CMAKE_CURRENT_SOURCE_DIR}/service"
        "${RS485_GENERATED_DIR}"
        ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(
    rs485_microservice
    PRIVATE
        gRPC::grpc++
        protobuf::libprotobuf
        Threads::Threads
)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        rs485_microservice
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )
endif()
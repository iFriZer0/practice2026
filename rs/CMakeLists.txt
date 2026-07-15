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
find_package(yaml-cpp REQUIRED)

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
    RS485_DRIVER_GENERATED_SOURCES
    "${RS485_GENERATED_DIR}/rs485_driver.pb.cc"
    "${RS485_GENERATED_DIR}/rs485_driver.grpc.pb.cc"
)

set(
    RS485_DRIVER_GENERATED_HEADERS
    "${RS485_GENERATED_DIR}/rs485_driver.pb.h"
    "${RS485_GENERATED_DIR}/rs485_driver.grpc.pb.h"
)

set(
    RS485_SERVICE_GENERATED_SOURCES
    "${RS485_GENERATED_DIR}/rs485_service.pb.cc"
    "${RS485_GENERATED_DIR}/rs485_service.grpc.pb.cc"
)

set(
    RS485_SERVICE_GENERATED_HEADERS
    "${RS485_GENERATED_DIR}/rs485_service.pb.h"
    "${RS485_GENERATED_DIR}/rs485_service.grpc.pb.h"
)

add_custom_command(
    OUTPUT
        ${RS485_DRIVER_GENERATED_SOURCES}
        ${RS485_DRIVER_GENERATED_HEADERS}
        ${RS485_SERVICE_GENERATED_SOURCES}
        ${RS485_SERVICE_GENERATED_HEADERS}

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
        ${RS485_DRIVER_GENERATED_SOURCES}
        ${RS485_DRIVER_GENERATED_HEADERS}
        ${RS485_SERVICE_GENERATED_SOURCES}
        ${RS485_SERVICE_GENERATED_HEADERS}
)

add_library(
    rs485_driver_proto
    STATIC

    ${RS485_DRIVER_GENERATED_SOURCES}
)

add_dependencies(
    rs485_driver_proto
    rs485_generate_proto
)

target_include_directories(
    rs485_driver_proto
    SYSTEM
    PUBLIC
        "${RS485_GENERATED_DIR}"
        ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(
    rs485_driver_proto
    PUBLIC
        gRPC::grpc++
        protobuf::libprotobuf
)

add_library(
    rs485_service_proto
    STATIC

    ${RS485_SERVICE_GENERATED_SOURCES}
)

add_dependencies(
    rs485_service_proto
    rs485_generate_proto
)

target_include_directories(
    rs485_service_proto
    SYSTEM
    PUBLIC
        "${RS485_GENERATED_DIR}"
        ${Protobuf_INCLUDE_DIRS}
)

target_link_libraries(
    rs485_service_proto
    PUBLIC
        gRPC::grpc++
        protobuf::libprotobuf
)

add_executable(
    rs485_microservice

    client/rs485_driver_client.cpp
    client/rs485_subscriber.cpp

    common/rs485_errors.cpp
    common/rs485_utils.cpp
    common/rs485_file_reader.cpp

    config/rs485_config.cpp

    service/rs485_service.cpp
    service/main.cpp
)

target_include_directories(
    rs485_microservice
    PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/client"
        "${CMAKE_CURRENT_SOURCE_DIR}/common"
        "${CMAKE_CURRENT_SOURCE_DIR}/config"
        "${CMAKE_CURRENT_SOURCE_DIR}/service"
)

target_link_libraries(
    rs485_microservice
    PRIVATE
        rs485_driver_proto
        rs485_service_proto
        Threads::Threads
        yaml-cpp
)

if(
    EXISTS
        "${CMAKE_CURRENT_SOURCE_DIR}/fake_driver/fake_rs485_driver.cpp"
    AND
    EXISTS
        "${CMAKE_CURRENT_SOURCE_DIR}/fake_driver/main.cpp"
)
    add_executable(
        rs485_fake_driver

        fake_driver/fake_rs485_driver.cpp
        fake_driver/main.cpp
    )

    target_include_directories(
        rs485_fake_driver
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/fake_driver"
    )

    target_link_libraries(
        rs485_fake_driver
        PRIVATE
            rs485_driver_proto
            Threads::Threads
    )
endif()

if(
    EXISTS
        "${CMAKE_CURRENT_SOURCE_DIR}/test_client/main.cpp"
)
    add_executable(
        rs485_test_client

        test_client/main.cpp
    )

    target_link_libraries(
        rs485_test_client
        PRIVATE
            rs485_service_proto
            Threads::Threads
    )
endif()

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(
        rs485_microservice
        PRIVATE
            -Wall
            -Wextra
            -Wpedantic
    )

    if(TARGET rs485_fake_driver)
        target_compile_options(
            rs485_fake_driver
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
        )
    endif()

    if(TARGET rs485_test_client)
        target_compile_options(
            rs485_test_client
            PRIVATE
                -Wall
                -Wextra
                -Wpedantic
        )
    endif()
endif()

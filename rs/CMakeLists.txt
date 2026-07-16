cmake_minimum_required(VERSION 3.16)

project(
    rs485_microservice
    VERSION 1.0.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(
    RS485_BUILD_TESTING
    "Build RS-485 fake driver and test client"
    OFF
)

find_package(
    Protobuf
    CONFIG
    REQUIRED
)

find_package(
    gRPC
    CONFIG
    REQUIRED
)

find_package(
    absl
    CONFIG
    REQUIRED
)

find_package(
    Threads
    REQUIRED
)

find_package(
    yaml-cpp
    CONFIG
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

if(TARGET protobuf::protoc)
    set(
        RS485_PROTOC_EXECUTABLE
        "$<TARGET_FILE:protobuf::protoc>"
    )
else()
    find_program(
        RS485_PROTOC_EXECUTABLE
        protoc
        REQUIRED
    )
endif()

if(TARGET gRPC::grpc_cpp_plugin)
    set(
        RS485_GRPC_CPP_PLUGIN
        "$<TARGET_FILE:gRPC::grpc_cpp_plugin>"
    )
else()
    find_program(
        RS485_GRPC_CPP_PLUGIN
        grpc_cpp_plugin
        REQUIRED
    )
endif()

add_custom_command(
    OUTPUT
        ${RS485_DRIVER_GENERATED_SOURCES}
        ${RS485_DRIVER_GENERATED_HEADERS}
        ${RS485_SERVICE_GENERATED_SOURCES}
        ${RS485_SERVICE_GENERATED_HEADERS}

    COMMAND
        "${RS485_PROTOC_EXECUTABLE}"

    ARGS
        "--proto_path=${RS485_API_DIR}"
        "--cpp_out=${RS485_GENERATED_DIR}"
        "--grpc_out=${RS485_GENERATED_DIR}"
        "--plugin=protoc-gen-grpc=${RS485_GRPC_CPP_PLUGIN}"
        "${RS485_DRIVER_PROTO}"
        "${RS485_SERVICE_PROTO}"

    DEPENDS
        "${RS485_DRIVER_PROTO}"
        "${RS485_SERVICE_PROTO}"

    COMMENT
        "Generating RS-485 protobuf and gRPC sources"

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
    ${RS485_DRIVER_GENERATED_HEADERS}
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
)

target_link_libraries(
    rs485_driver_proto
    PUBLIC
        gRPC::grpc++
        protobuf::libprotobuf

        absl::log
        absl::check
)

add_library(
    rs485_service_proto
    STATIC

    ${RS485_SERVICE_GENERATED_SOURCES}
    ${RS485_SERVICE_GENERATED_HEADERS}
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
)

target_link_libraries(
    rs485_service_proto
    PUBLIC
        gRPC::grpc++
        protobuf::libprotobuf

        absl::log
        absl::check
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

if(RS485_BUILD_TESTING)
    set(
        RS485_FAKE_DRIVER_SOURCES
        "fake_driver/fake_rs485_driver.cpp"
        "fake_driver/main.cpp"
        "config/rs485_config.cpp"
    )

    set(
        RS485_TEST_CLIENT_SOURCES
        "test_client/main.cpp"
    )

    foreach(
        RS485_REQUIRED_SOURCE
        IN LISTS
            RS485_FAKE_DRIVER_SOURCES
            RS485_TEST_CLIENT_SOURCES
    )
        if(
            NOT EXISTS
            "${CMAKE_CURRENT_SOURCE_DIR}/${RS485_REQUIRED_SOURCE}"
        )
            message(
                FATAL_ERROR
                "Required RS-485 testing source was not found: "
                "${RS485_REQUIRED_SOURCE}"
            )
        endif()
    endforeach()

    if(
        NOT EXISTS
        "${CMAKE_CURRENT_SOURCE_DIR}/fake_driver/fake_rs485_driver.h"
    )
        message(
            FATAL_ERROR
            "Required RS-485 testing header was not found: "
            "fake_driver/fake_rs485_driver.h"
        )
    endif()

    add_executable(
        rs485_fake_driver

        fake_driver/fake_rs485_driver.h
        ${RS485_FAKE_DRIVER_SOURCES}
    )

    target_include_directories(
        rs485_fake_driver
        PRIVATE
            "${CMAKE_CURRENT_SOURCE_DIR}/fake_driver"
            "${CMAKE_CURRENT_SOURCE_DIR}/config"
    )

    target_link_libraries(
        rs485_fake_driver
        PRIVATE
            rs485_driver_proto
            Threads::Threads
            yaml-cpp
    )

    add_executable(
        rs485_test_client

        ${RS485_TEST_CLIENT_SOURCES}
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

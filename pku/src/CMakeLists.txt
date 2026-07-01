cmake_minimum_required(VERSION 3.22)

project(pku)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(GRPC_GEN_DIR ${CMAKE_SOURCE_DIR}/grpc_ui_service)

add_executable(
    service
    ${GRPC_GEN_DIR}/pku_service.pb.cc
    ${GRPC_GEN_DIR}/pku_service.grpc.pb.cc
    service/grpc_pku_service.cpp
    service/main.cpp
)

target_include_directories(
    service
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${GRPC_GEN_DIR}
    ${Protobuf_INCLUDE_DIR}
)

target_link_libraries(
    service
    gRPC::grpc++
    gRPC::grpc
    ${Protobuf_LIBRARIES}
)

add_executable(
    ui
    ${GRPC_GEN_DIR}/pku_service.pb.cc
    ${GRPC_GEN_DIR}/pku_service.grpc.pb.cc
    ui/main.cpp
)

target_include_directories(
    ui
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${GRPC_GEN_DIR}
    ${Protobuf_INCLUDE_DIR}
)

target_link_libraries(
    ui
    gRPC::grpc++
    gRPC::grpc
    ${Protobuf_LIBRARIES}
)

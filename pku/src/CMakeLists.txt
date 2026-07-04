cmake_minimum_required(VERSION 3.22)

project(pku)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

file(GLOB_RECURSE DRIVER_SOURCES CONFIGURE_DEPENDS "driver/*.cpp")
set(GRPC_PKU_DRIVER_DIRECTORY ${CMAKE_SOURCE_DIR}/grpc_pku_driver_cpp)

add_executable(
    driver
    ${DRIVER_SOURCES}
    ${GRPC_PKU_DRIVER_DIRECTORY}/pku_driver_test.pb.cc
    ${GRPC_PKU_DRIVER_DIRECTORY}/pku_driver_test.grpc.pb.cc
)

target_include_directories(
    driver
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${GRPC_PKU_DRIVER_DIRECTORY}
    ${Protobuf_INCLUDE_DIR}
)

target_link_libraries(
    driver
    gRPC::grpc++
    gRPC::grpc
    ${Protobuf_LIBRARIES}
)

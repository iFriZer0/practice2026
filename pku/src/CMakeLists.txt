cmake_minimum_required(VERSION 3.22)

project(pku)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(absl_DIR "/home/alexander/gRPC/lib/cmake/absl" CACHE PATH "Abseil configuration directory")
set(utf8_range_DIR "/home/alexander/gRPC/lib/cmake/utf8_range" CACHE PATH "utf8_range configuration directory")
set(Protobuf_DIR "/home/alexander/gRPC/lib/cmake/protobuf" CACHE PATH "Protobuf configuration directory")
set(gRPC_DIR "/home/alexander/gRPC/lib/cmake/grpc" CACHE PATH "gRPC configuration directory")

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

set(GRPC_BIN_DIR "/home/alexander/gRPC/bin")
find_program(PROTOC_EXECUTABLE protoc PATHS ${GRPC_BIN_DIR} NO_DEFAULT_PATH REQUIRED)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin PATHS ${GRPC_BIN_DIR} NO_DEFAULT_PATH REQUIRED)

set(PROTO_FILE ${CMAKE_SOURCE_DIR}/api/pku_service.proto)
set(GRPC_GEN_DIR ${CMAKE_SOURCE_DIR}/grpc_ui_service)
file(MAKE_DIRECTORY ${GRPC_GEN_DIR})

set(
    GENERATED_FILES
    ${GRPC_GEN_DIR}/pku_service.pb.cc
    ${GRPC_GEN_DIR}/pku_service.pb.h
    ${GRPC_GEN_DIR}/pku_service.grpc.pb.cc
    ${GRPC_GEN_DIR}/pku_service.grpc.pb.h
)

add_custom_command(
    OUTPUT ${GENERATED_FILES}
    COMMAND ${PROTOC_EXECUTABLE} -I=${CMAKE_SOURCE_DIR}/api --cpp_out=${GRPC_GEN_DIR} --grpc_out=${GRPC_GEN_DIR} --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} ${PROTO_FILE}
    DEPENDS ${PROTO_FILE}
    COMMENT "Generating gRPC code from ${PROTO_FILE}"
    VERBATIM
)

add_library(pku_proto_objects OBJECT ${GENERATED_FILES})

target_include_directories(pku_proto_objects PRIVATE ${GRPC_GEN_DIR})

target_link_libraries(pku_proto_objects PUBLIC protobuf::libprotobuf)

set_target_properties(pku_proto_objects PROPERTIES POSITION_INDEPENDENT_CODE ON)

add_executable(
    service
    $<TARGET_OBJECTS:pku_proto_objects>
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
    $<TARGET_OBJECTS:pku_proto_objects>
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

cmake_minimum_required(VERSION 3.22)

project(pku)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# В PATH должен быть путь к исполняемым файлам gRPC (.../gRPC/bin).
set(PROTOC_EXECUTABLE protoc)
set(GRPC_CPP_PLUGIN "`which grpc_cpp_plugin`")

set(UI_SERVICE_API ${CMAKE_SOURCE_DIR}/api/ui_service.proto)
set(GRPC_UI_SERVICE ${CMAKE_SOURCE_DIR}/grpc_ui_service)
file(MAKE_DIRECTORY ${GRPC_UI_SERVICE})

set(
    UI_SERVICE_GENERATED
    ${GRPC_UI_SERVICE}/ui_service.pb.cc
    ${GRPC_UI_SERVICE}/ui_service.pb.h
    ${GRPC_UI_SERVICE}/ui_service.grpc.pb.cc
    ${GRPC_UI_SERVICE}/ui_service.grpc.pb.h
)

add_custom_command(
    OUTPUT ${UI_SERVICE_GENERATED}
    COMMAND bash -c "${PROTOC_EXECUTABLE} -I=\"${CMAKE_SOURCE_DIR}/api\" --cpp_out=\"${GRPC_UI_SERVICE}\" --grpc_out=\"${GRPC_UI_SERVICE}\" --plugin=protoc-gen-grpc=\"${GRPC_CPP_PLUGIN}\" \"${UI_SERVICE_API}\""
    DEPENDS ${UI_SERVICE_API}
    COMMENT "Generating gRPC code from ${UI_SERVICE_API}"
    VERBATIM
)

add_library(ui_service_objects OBJECT ${UI_SERVICE_GENERATED})

target_include_directories(ui_service_objects PRIVATE ${GRPC_UI_SERVICE})

target_link_libraries(ui_service_objects PUBLIC protobuf::libprotobuf)

set_target_properties(ui_service_objects PROPERTIES POSITION_INDEPENDENT_CODE ON)

file(GLOB_RECURSE SERVICE_SOURCES CONFIGURE_DEPENDS "service/*.cpp")
file(GLOB_RECURSE UI_SOURCES CONFIGURE_DEPENDS "ui/*.cpp")

add_executable(
    service
    $<TARGET_OBJECTS:ui_service_objects>
    ${SERVICE_SOURCES}
)

target_include_directories(
    service
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${GRPC_UI_SERVICE}
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
    $<TARGET_OBJECTS:ui_service_objects>
    ${UI_SOURCES}
)

target_include_directories(
    ui
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${GRPC_UI_SERVICE}
    ${Protobuf_INCLUDE_DIR}
)

target_link_libraries(
    ui
    gRPC::grpc++
    gRPC::grpc
    ${Protobuf_LIBRARIES}
)

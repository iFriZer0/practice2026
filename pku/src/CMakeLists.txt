cmake_minimum_required(VERSION 3.22)

project(pku)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)

# В PATH должен быть путь к исполняемым файлам gRPC (.../gRPC/bin).
find_program(PROTOC_EXECUTABLE protoc REQUIRED)
find_program(GRPC_CPP_PLUGIN grpc_cpp_plugin REQUIRED)

macro(generate_grpc NAME API OUTPUT_DIRECTORY GENERATED)
    file(MAKE_DIRECTORY ${OUTPUT_DIRECTORY})
    set(
        ${GENERATED}
        ${OUTPUT_DIRECTORY}/${NAME}.pb.cc
        ${OUTPUT_DIRECTORY}/${NAME}.pb.h
        ${OUTPUT_DIRECTORY}/${NAME}.grpc.pb.cc
        ${OUTPUT_DIRECTORY}/${NAME}.grpc.pb.h
    )
    add_custom_command(
        OUTPUT ${${GENERATED}}
        COMMAND ${PROTOC_EXECUTABLE} -I=${CMAKE_SOURCE_DIR}/api --cpp_out=${OUTPUT_DIRECTORY} --grpc_out=${OUTPUT_DIRECTORY} --plugin=protoc-gen-grpc=${GRPC_CPP_PLUGIN} ${API}
        DEPENDS ${API}
        COMMENT "Generating gRPC code from ${API}"
        VERBATIM
    )
    add_library(${NAME}_objects OBJECT ${${GENERATED}})
    target_include_directories(${NAME}_objects PRIVATE ${OUTPUT_DIRECTORY})
    target_link_libraries(${NAME}_objects PUBLIC protobuf::libprotobuf)
    set_target_properties(${NAME}_objects PROPERTIES POSITION_INDEPENDENT_CODE ON)
endmacro()

set(GRPC_SERVICE_DRIVER ${CMAKE_SOURCE_DIR}/grpc_service_driver)
generate_grpc(
    service_driver
    ${CMAKE_SOURCE_DIR}/api/service_driver.proto
    ${GRPC_SERVICE_DRIVER}
    SERVICE_DRIVER_GENERATED
)

set(GRPC_UI_SERVICE ${CMAKE_SOURCE_DIR}/grpc_ui_service)
generate_grpc(
    ui_service
    ${CMAKE_SOURCE_DIR}/api/ui_service.proto
    ${GRPC_UI_SERVICE}
    UI_SERVICE_GENERATED
)

file(GLOB_RECURSE DRIVER_SOURCES CONFIGURE_DEPENDS "driver/*.cpp")
file(GLOB_RECURSE SERVICE_SOURCES CONFIGURE_DEPENDS "service/*.cpp")
file(GLOB_RECURSE UI_SOURCES CONFIGURE_DEPENDS "ui/*.cpp")

add_executable(
    driver
    $<TARGET_OBJECTS:service_driver_objects>
    ${DRIVER_SOURCES}
)

target_include_directories(
    driver
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${GRPC_SERVICE_DRIVER}
    ${Protobuf_INCLUDE_DIR}
)

target_link_libraries(
    driver
    gRPC::grpc++
    gRPC::grpc
    ${Protobuf_LIBRARIES}
)

add_executable(
    service
    $<TARGET_OBJECTS:service_driver_objects>
    $<TARGET_OBJECTS:ui_service_objects>
    ${SERVICE_SOURCES}
)

target_include_directories(
    service
    PRIVATE
    ${CMAKE_SOURCE_DIR}
    ${GRPC_SERVICE_DRIVER}
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

QT += core gui widgets

CONFIG += c++20

GRPC_CFLAGS = $$system(pkg-config --cflags grpc++ protobuf)
GRPC_LIBS = $$system(pkg-config --static --libs grpc++ protobuf)

QMAKE_CXXFLAGS += $$GRPC_CFLAGS
LIBS += $$GRPC_LIBS

QMAKE_CXXFLAGS += \
    -Wall \
    -Werror \
    -Wpedantic \
    -Wextra \
    -Wfloat-conversion \
    -Wfloat-equal

PKG_CONFIG_PATHS = $$system(pkg-config --cflags-only-I grpc++ protobuf)
PKG_CONFIG_SYSTEM_PATHS = $$replace(PKG_CONFIG_PATHS, -I, -isystem)

QMAKE_CXXFLAGS += $$PKG_CONFIG_SYSTEM_PATHS

defineTest(generateGRPC) {
    outputDirectory = $$1
    script = $$2
    api = $$3

    !exists($$script) {
        error("Generation script $$script was not found")
    }

    !exists($$api) {
        error("Proto file $$api was not found")
    }

    generationResult = $$system("\"$$script\"")

    base = $$basename(api)
    base = $$replace(base, \\.proto$,)

    protoSource = $$outputDirectory/$${base}.pb.cc
    protoHeader = $$outputDirectory/$${base}.pb.h
    grpcSource = $$outputDirectory/$${base}.grpc.pb.cc
    grpcHeader = $$outputDirectory/$${base}.grpc.pb.h

    !exists($$protoSource) {
        error("Generated file $$protoSource was not found")
    }

    !exists($$protoHeader) {
        error("Generated file $$protoHeader was not found")
    }

    !exists($$grpcSource) {
        error("Generated file $$grpcSource was not found")
    }

    !exists($$grpcHeader) {
        error("Generated file $$grpcHeader was not found")
    }

    return(true)
}

RS485_ROOT_DIR = $$clean_path($$PWD/../../rs)
RS485_CLIENT_DIR = $$clean_path($$RS485_ROOT_DIR/client)
RS485_GENERATED_DIR = $$clean_path($$RS485_ROOT_DIR/build/gui_generated)
RS485_GENERATE_SCRIPT = $$clean_path($$RS485_ROOT_DIR/scripts/generate_gui_proto.sh)
RS485_API = $$clean_path($$RS485_ROOT_DIR/api/rs485_service.proto)

!generateGRPC($$RS485_GENERATED_DIR, $$RS485_GENERATE_SCRIPT, $$RS485_API) {
    error("Failed to generate RS-485 gRPC code")
}

RS485_PROTO_SOURCE = $$RS485_GENERATED_DIR/rs485_service.pb.cc
RS485_PROTO_HEADER = $$RS485_GENERATED_DIR/rs485_service.pb.h
RS485_GRPC_SOURCE = $$RS485_GENERATED_DIR/rs485_service.grpc.pb.cc
RS485_GRPC_HEADER = $$RS485_GENERATED_DIR/rs485_service.grpc.pb.h

INCLUDEPATH += \
    $$PWD \
    $$PWD/graphical_views \
    $$PWD/graphical_views/widgets \
    $$PWD/graphical_views/views \
    $$PWD/graphical_views/views/main \
    $$PWD/factory \
    $$PWD/factory/errors \
    $$PWD/errors \
    $$PWD/application \
    $$PWD/application/factory \
    $$PWD/application/factory/errors \
    $$RS485_CLIENT_DIR \
    $$RS485_GENERATED_DIR

DEPENDPATH += \
    $$PWD \
    $$PWD/graphical_views \
    $$PWD/graphical_views/widgets \
    $$PWD/graphical_views/views \
    $$PWD/graphical_views/views/main \
    $$PWD/factory \
    $$PWD/factory/errors \
    $$PWD/errors \
    $$PWD/application \
    $$PWD/application/factory \
    $$PWD/application/factory/errors \
    $$RS485_CLIENT_DIR \
    $$RS485_GENERATED_DIR

SOURCES += \
    application/application.cpp \
    application/factory/errors/qt_main_view_builder_factory_error.cpp \
    application/factory/errors/qt_main_view_builder_memory_error.cpp \
    application/factory/errors/view_builder_error.cpp \
    application/factory/errors/view_director_build_error.cpp \
    application/factory/errors/view_director_error.cpp \
    application/factory/qt_main_view_builder.cpp \
    application/factory/view_director.cpp \
    errors/error.cpp \
    factory/errors/creator_error.cpp \
    factory/errors/creator_maker_error.cpp \
    factory/errors/simple_creator_error.cpp \
    factory/errors/simple_creator_maker_error.cpp \
    factory/errors/simple_creator_maker_memory_error.cpp \
    factory/errors/simple_creator_memory_error.cpp \
    factory/errors/solution_error.cpp \
    factory/errors/solution_memory_error.cpp \
    graphical_views/views/main/qt_main_view.cpp \
    graphical_views/views/main/qt_view_mko.cpp \
    graphical_views/views/main/qt_view_pku.cpp \
    graphical_views/views/main/qt_view_rs_485.cpp \
    graphical_views/widgets/main_window.cpp \
    $$RS485_CLIENT_DIR/rs485_microservice_client.cpp \
    $$RS485_PROTO_SOURCE \
    $$RS485_GRPC_SOURCE \
    main.cpp

HEADERS += \
    application/application.h \
    application/factory/errors/qt_main_view_builder_factory_error.h \
    application/factory/errors/qt_main_view_builder_memory_error.h \
    application/factory/errors/view_builder_error.h \
    application/factory/errors/view_director_build_error.h \
    application/factory/errors/view_director_error.h \
    application/factory/qt_main_view_builder.h \
    application/factory/view_builder.h \
    application/factory/view_director.h \
    errors/error.h \
    factory/creator.h \
    factory/creator_maker.h \
    factory/errors/creator_error.h \
    factory/errors/creator_maker_error.h \
    factory/errors/simple_creator_error.h \
    factory/errors/simple_creator_maker_error.h \
    factory/errors/simple_creator_maker_memory_error.h \
    factory/errors/simple_creator_memory_error.h \
    factory/errors/solution_error.h \
    factory/errors/solution_memory_error.h \
    factory/errors/solution_no_maker_error.h \
    factory/errors/solution_no_maker_error.hpp \
    factory/simple_creator.h \
    factory/simple_creator.hpp \
    factory/simple_creator_maker.h \
    factory/simple_creator_maker.hpp \
    factory/solution.h \
    factory/solution.hpp \
    graphical_views/views/main/qt_main_view.h \
    graphical_views/views/main/qt_view_mko.h \
    graphical_views/views/main/qt_view_pku.h \
    graphical_views/views/main/qt_view_rs_485.h \
    graphical_views/views/main/view.h \
    graphical_views/widgets/main_window.h \
    $$RS485_CLIENT_DIR/rs485_gui_types.h \
    $$RS485_CLIENT_DIR/rs485_microservice_client.h \
    $$RS485_PROTO_HEADER \
    $$RS485_GRPC_HEADER

QMAKE_CLEAN += \
    $$RS485_PROTO_SOURCE \
    $$RS485_PROTO_HEADER \
    $$RS485_GRPC_SOURCE \
    $$RS485_GRPC_HEADER

qnx {
    target.path = /tmp/$${TARGET}/bin
} else: unix:!android {
    target.path = /opt/$${TARGET}/bin
}

!isEmpty(target.path) {
    INSTALLS += target
}

DISTFILES += \
    .clangd \
    $$RS485_API \
    $$RS485_GENERATE_SCRIPT

DESTDIR = $$PWD/build/out
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
UI_DIR = $$PWD/build/ui
RCC_DIR = $$PWD/build/rcc
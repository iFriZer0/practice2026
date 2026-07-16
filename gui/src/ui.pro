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

macx: QMAKE_CXXFLAGS += -Wno-nullability-extension

PKG_CONFIG_PATHS = $$system(pkg-config --cflags-only-I grpc++ protobuf)
PKG_CONFIG_SYSTEM_PATHS = $$replace(PKG_CONFIG_PATHS, -I, -isystem )

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

    SOURCES += $$protoSource $$grpcSource
    HEADERS += $$protoHeader $$grpcHeader
    INCLUDEPATH += $$outputDirectory
    DEPENDPATH += $$outputDirectory

    export(SOURCES)
    export(HEADERS)
    export(INCLUDEPATH)
    export(DEPENDPATH)

    return(true)
}

MKO_ROOT_DIR = $$clean_path($$PWD/../../mko)
MKO_GENERATED_DIR = $$clean_path($$PWD/generated/mko)
MKO_GENERATE_SCRIPT = $$clean_path($$MKO_ROOT_DIR/api/generate-gui-cpp.sh)
MKO_API = $$clean_path($$MKO_ROOT_DIR/api/mko.proto)

!generateGRPC($$MKO_GENERATED_DIR, $$MKO_GENERATE_SCRIPT, $$MKO_API) {
    error("Failed to generate MKO gRPC code")
}

QMAKE_CLEAN += -rf $$MKO_GENERATED_DIR

PKU_ROOT_DIR = $$clean_path($$PWD/../../pku)
PKU_GENERATED_DIR = $$clean_path($$PWD/grpc_pku_service)
PKU_GENERATE_SCRIPT = $$clean_path($$PKU_ROOT_DIR/src/api/generate.sh)
PKU_API = $$clean_path($$PKU_ROOT_DIR/src/api/pku_service.proto)

!generateGRPC($$PKU_GENERATED_DIR, $$PKU_GENERATE_SCRIPT, $$PKU_API) {
    error("Failed to generate Pku gRPC code")
}

RS485_ROOT_DIR = $$clean_path($$PWD/../../rs)
RS485_CLIENT_DIR = $$clean_path($$RS485_ROOT_DIR/client)
RS485_GENERATED_DIR = $$clean_path($$RS485_ROOT_DIR/build/gui_generated)
RS485_GENERATE_SCRIPT = $$clean_path($$RS485_ROOT_DIR/scripts/generate_gui_proto.sh)
RS485_API = $$clean_path($$RS485_ROOT_DIR/api/rs485_service.proto)

!generateGRPC($$RS485_GENERATED_DIR, $$RS485_GENERATE_SCRIPT, $$RS485_API) {
    error("Failed to generate RS-485 gRPC code")
}

QMAKE_CLEAN += -rf $$PKU_GENERATED_DIR
QMAKE_CLEAN += -rf $$RS485_GENERATED_DIR

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
    $$PWD/clients/mko \
    $$MKO_GENERATED_DIR \
    $$RS485_CLIENT_DIR \
    $$RS485_GENERATED_DIR \
    $$PKU_GENERATED_DIR

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
    $$PWD/clients/mko \
    $$MKO_GENERATED_DIR \
    $$RS485_CLIENT_DIR \
    $$RS485_GENERATED_DIR \
    $$PKU_GENERATED_DIR

SOURCES += \
    application/application.cpp \
    application/factory/errors/qt_main_view_builder_factory_error.cpp \
    application/factory/errors/qt_main_view_builder_memory_error.cpp \
    application/factory/errors/view_builder_error.cpp \
    application/factory/errors/view_director_build_error.cpp \
    application/factory/errors/view_director_error.cpp \
    application/factory/qt_main_view_builder.cpp \
    application/factory/view_director.cpp \
    clients/mko/grpc_mko_client.cpp \
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
    clients/mko/grpc_mko_client.h \
    clients/mko/mko_client.h \
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
    $$RS485_CLIENT_DIR/rs485_microservice_client.h

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
    $$MKO_API \
    $$MKO_GENERATE_SCRIPT \
    $$RS485_API \
    $$RS485_GENERATE_SCRIPT \
    $$PKU_API \
    $$PKU_GENERATE_SCRIPT

DESTDIR = $$PWD/build/out
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
UI_DIR = $$PWD/build/ui
RCC_DIR = $$PWD/build/rcc

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
PKG_CONFIG_SYSTEM_PATHS = $$replace(PKG_CONFIG_PATHS, -I, -isystem )

QMAKE_CXXFLAGS += $$PKG_CONFIG_SYSTEM_PATHS

defineTest(generateGRPC) {
    outputDirectory = $$1
    script = $$2
    api = $$3
    !exists($$script) {
        error("Generation script $$script was not found")
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
        error("Generated file $$protoSource was not found")
    }
    !exists($$grpcSource) {
        error("Generated file $$protoSource was not found")
    }
    !exists($$grpcHeader) {
        error("Generated file $$protoSource was not found")
    }
    SOURCES += $$protoSource $$grpcSource
    HEADERS += $$protoHeader $$grpcHeader
    INCLUDEPATH += $$outputDirectory
    DEPENDPATH += $$outputDirectory
    return (true)
}

PKU_ROOT_DIR = $$clean_path($$PWD/../../pku)
PKU_GENERATED_DIR = $$clean_path($$PWD/grpc_pku_service)
PKU_GENERATE_SCRIPT = $$clean_path($$PKU_ROOT_DIR/src/api/generate.sh)
PKU_API = $$clean_path($$PKU_ROOT_DIR/src/api/pku_service.proto)

!generateGRPC($$PKU_GENERATED_DIR, $$PKU_GENERATE_SCRIPT, $$PKU_API) {
    error("Failed to generate Pku gRPC code")
}

QMAKE_CLEAN += -rf $$PWD/grpc_pku_service

INCLUDEPATH += \
    graphical_views \
    graphical_views/widgets \
    graphical_views/views \
    graphical_views/views/main \
    factory \
    factory/errors \
    errors \
    application \
    application/factory \
    application/factory/errors \
    grpc_pku_service/

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
    grpc_pku_service/pku_service.grpc.pb.cc \
    grpc_pku_service/pku_service.pb.cc \
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
    grpc_pku_service/pku_service.grpc.pb.h \
    grpc_pku_service/pku_service.pb.h

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
    .clangd \

DESTDIR = $$PWD/build/out
OBJECTS_DIR = $$PWD/build/obj
MOC_DIR = $$PWD/build/moc
UI_DIR = $$PWD/build/ui
RCC_DIR = $$PWD/build/rcc

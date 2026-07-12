QT       += core gui widgets

CONFIG += c++20

QMAKE_CXXFLAGS += -Wall -Werror -Wpedantic -Wextra -Wfloat-conversion -Wfloat-equal -Wno-nullability-extension -Wno-deprecated-declarations -Wno-gcc-compat -Wno-gnu-anonymous-struct -Wno-nested-anon-types

INCLUDEPATH += \
    /opt/homebrew/include \
    /usr/local/include \
    graphical_views/ \
    graphical_views/widgets \
    graphical_views/views \
    graphical_views/views/main \
    clients/mko \
    generated/mko \
    factory \
    factory/errors \
    errors \
    application \
    application/factory \
    application/factory/errors

LIBS += \
    -L/opt/homebrew/lib \
    -L/usr/local/lib \
    -lgrpc++ \
    -lgrpc \
    -lprotobuf \
    -lgpr

macx: LIBS += $$system(find /opt/homebrew/lib -maxdepth 1 -name 'libabsl_*.dylib' ! -name '*.2601.*' -print)
linux: LIBS += $$system(find /usr/lib -maxdepth 1 -name 'libabsl_*.so' -print)

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
    generated/mko/mko.grpc.pb.cc \
    generated/mko/mko.pb.cc \
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
    generated/mko/mko.grpc.pb.h \
    generated/mko/mko.pb.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .clangd

DESTDIR = $$PWD/build/out

OBJECTS_DIR = $$PWD/build/obj

MOC_DIR = $$PWD/build/moc

UI_DIR = $$PWD/build/ui

RCC_DIR = $$PWD/build/rcc

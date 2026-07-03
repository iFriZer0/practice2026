QT       += core gui widgets

CONFIG += c++20

QMAKE_CXXFLAGS += -Wall -Werror -Wpedantic -Wextra -Wfloat-conversion -Wfloat-equal

INCLUDEPATH += \
    graphical_views/ \
    graphical_views/widgets \
    graphical_views/views \
    graphical_views/views/main \
    factory \
    factory/errors \
    errors \
    application

SOURCES += \
    application/application.cpp \
    error/error.cpp \
    errors/error.cpp \
    factory/errors/creator_maker_error.cpp \
    factory/errors/creator_maker_memory_error.cpp \
    graphical_views/views/main/qt_main_view.cpp \
    graphical_views/widgets/main_window.cpp \
    main.cpp

HEADERS += \
    application/application.h \
    error/error.h \
    errors/error.h \
    factory/creator.h \
    factory/creator_maker.h \
    factory/creator_maker.hpp \
    factory/errors/creator_maker_error.h \
    factory/errors/creator_maker_memory_error.h \
    factory/solution.h \
    graphical_views/views/main/qt_main_view.h \
    graphical_views/views/main/view.h \
    graphical_views/widgets/main_window.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    .clangd

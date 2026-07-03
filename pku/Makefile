CXX = g++
CXXFLAGS = -std=c++17 -Wall

all: ui_test

ui_test: src/ui/main.cpp src/ui/mainwindow.cpp src/ui/mainwindow.h
              $(CXX) $(CXXFLAGS) src/ui/main.cpp src/ui/mainwindow.cpp -o ui_test

clean:
    rm -f ui_test

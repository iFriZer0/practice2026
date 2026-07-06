#include <iostream>
#include <memory>
#include <new>
#include <stdexcept>
#include <QWidget>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QTabBar>
#include <QObject>
#include "main_window.h"

MainWindow::MainWindow(
        const std::shared_ptr<View> &view_rs_485,
        const std::shared_ptr<View> &view_mko,
        const std::shared_ptr<View> &view_pku,
        QStackedWidget *const stacked_widget,
        QWidget *parent
)
    : QMainWindow(parent),
      tab_bar{create_tab_bar()},
      stacked_widget{stacked_widget},
      views{{TAB_RS_485, view_rs_485}, {TAB_MKO, view_mko}, {TAB_PKU, view_pku}}
{
    QWidget *central{create_widget()};
    setCentralWidget(central);
    tab_bar->addTab("RS-485");
    tab_bar->addTab("МКО");
    tab_bar->addTab("РК/ПКУ");
    QVBoxLayout *layout{create_v_box_layout(central)};
    layout->addWidget(tab_bar);
    layout->addWidget(stacked_widget);
    QObject::connect(tab_bar, &QTabBar::currentChanged, this, &MainWindow::switch_tab);
    switch_tab(TAB_RS_485);
}

QTabBar *MainWindow::create_tab_bar() const
{
    QTabBar *tab_bar{nullptr};
    try
    {
        tab_bar = new QTabBar{};
    }
    catch (const std::bad_alloc &)
    {
        std::cerr << "Не удалось создать QTabBar." << std::endl;
    }
    return tab_bar;
}

QWidget *MainWindow::create_widget() const
{
    QWidget *widget{nullptr};
    try
    {
        widget = new QWidget{};
    }
    catch (const std::bad_alloc &)
    {
        std::cerr << "Не удалось создать QWidget." << std::endl;
    }
    return widget;
}

QVBoxLayout *MainWindow::create_v_box_layout(QWidget *const parent) const
{
    QVBoxLayout *layout{nullptr};
    try
    {
        layout = new QVBoxLayout{parent};
    }
    catch (const std::bad_alloc &)
    {
        std::cerr << "Не удалось создать QVBoxLayout." << std::endl;
    }

    return layout;
}

void MainWindow::switch_tab(int index)
{
    try
    {
        views.at(index)->show();
    }
    catch (const std::out_of_range &) {}
}

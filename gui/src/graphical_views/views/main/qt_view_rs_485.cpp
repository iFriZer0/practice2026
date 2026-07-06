#include <iostream>
#include <new>
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include "qt_view_rs_485.h"

QtViewRS485::QtViewRS485(QStackedWidget *const stacked_widget) noexcept
    : stacked_widget{stacked_widget}
{
    central_widget = create_widget();
    QVBoxLayout *layout = create_v_box_layout(central_widget);
    layout->addWidget(create_label("RS-485"));
    central_widget->setLayout(layout);
}

void QtViewRS485::show()
{
    if (stacked_widget->indexOf(central_widget) == NOT_FOUND)
    {
        stacked_widget->addWidget(central_widget);
    }
    stacked_widget->setCurrentWidget(central_widget);
}

QWidget *QtViewRS485::create_widget() const
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

QVBoxLayout *QtViewRS485::create_v_box_layout(QWidget *const parent) const
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

QLabel *QtViewRS485::create_label(const QString &text) const
{
    QLabel *label{nullptr};
    try
    {
        label = new QLabel{text};
    }
    catch (const std::bad_alloc &)
    {
        std::cerr << "Не удалось создать QLabel." << std::endl;
    }

    return label;
}

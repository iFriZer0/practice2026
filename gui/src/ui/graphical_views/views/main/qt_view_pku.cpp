#include <iostream>
#include <new>
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include "qt_view_pku.h"

QtViewPKU::QtViewPKU(QStackedWidget *const stacked_widget) noexcept
    : stacked_widget{stacked_widget}
{
    central_widget = create_widget();
    QVBoxLayout *layout = create_v_box_layout(central_widget);
    layout->addWidget(create_label("РК/ПКУ"));
    central_widget->setLayout(layout);
}

void QtViewPKU::show()
{
    if (stacked_widget->indexOf(central_widget) == NOT_FOUND)
    {
        stacked_widget->addWidget(central_widget);
    }
    stacked_widget->setCurrentWidget(central_widget);
}

QWidget *QtViewPKU::create_widget() const
{
    QWidget *widget;
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

QVBoxLayout *QtViewPKU::create_v_box_layout(QWidget *const parent) const
{
    QVBoxLayout *layout;
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

QLabel *QtViewPKU::create_label(const QString &text) const
{
    QLabel *label;
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

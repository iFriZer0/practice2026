#include <QComboBox>
#include <QTextEdit>
#include <iostream>
#include <new>
#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QPushButton>
#include <QLineEdit>
#include <QHBoxLayout>
#include <QCheckBox>
#include "qt_view_pku.h"

QtViewPKU::QtViewPKU(QStackedWidget *const stacked_widget) noexcept
    : stacked_widget{stacked_widget}
{
    central_widget = create_widget();
    QVBoxLayout *main_layout = create_v_box_layout(central_widget);

    main_layout->addWidget(create_label("Пульт контроля и управления (РК/ПКУ)"));

    main_layout->addWidget(create_label("Выдача РК:"));
    QHBoxLayout *rk_layout = new QHBoxLayout();

    QLabel *rk_num_label = create_label("Номер РК:");
    QLineEdit *rk_num_input = new QLineEdit(central_widget);
    rk_num_input->setMaximumWidth(60);

    QLabel *rk_time_label = create_label("Длительность (мс):");
    QLineEdit *rk_time_input = new QLineEdit(central_widget);
    rk_time_input->setMaximumWidth(80);

    QPushButton *btn_send_rk = new QPushButton("Выдать РК", central_widget);

    rk_layout->addWidget(rk_num_label);
    rk_layout->addWidget(rk_num_input);
    rk_layout->addWidget(rk_time_label);
    rk_layout->addWidget(rk_time_input);
    rk_layout->addWidget(btn_send_rk);
    main_layout->addLayout(rk_layout);

    main_layout->addWidget(create_label("Настройка срабатывания ПКУ (1-64):"));
    QHBoxLayout *cfg_layout = new QHBoxLayout();

    QLabel *chan_label = create_label("Выбор ПКУ:");
    QComboBox *chan_select = new QComboBox(central_widget);
    for (int i = 1; i <= 64; ++i) {
        chan_select->addItem(QString("ПКУ %1").arg(i));
    }

    QLabel *mode_label = create_label("Режим:");
    QComboBox *mode_select = new QComboBox(central_widget);
    mode_select->addItem("0 – Отрицательный импульс");
    mode_select->addItem("1 – Положительный импульс");
    mode_select->addItem("2 – Фронт");

    QPushButton *btn_set_mode = new QPushButton("Применить режим", central_widget);

    cfg_layout->addWidget(chan_label);
    cfg_layout->addWidget(chan_select);
    cfg_layout->addWidget(mode_label);
    cfg_layout->addWidget(mode_select);
    cfg_layout->addWidget(btn_set_mode);
    main_layout->addLayout(cfg_layout);

    main_layout->addWidget(create_label("Измеренная длительность импульсов ПКУ:"));

    QTextEdit *pku_log = new QTextEdit(central_widget);
    pku_log->setReadOnly(true);
    main_layout->addWidget(pku_log);

    QLabel *status_label = create_label("Статус ПКУ: устройство готово к работе");
    main_layout->addWidget(status_label);

    central_widget->setLayout(main_layout);
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

QVBoxLayout *QtViewPKU::create_v_box_layout(QWidget *const parent) const
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

QLabel *QtViewPKU::create_label(const QString &text) const
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

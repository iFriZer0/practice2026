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

    // Верхние текстовые метки (без двоеточий)
    main_layout->addWidget(create_label("Пульт контроля и управления (РК/ПКУ)"));
    main_layout->addWidget(create_label("Выдача РК"));
    main_layout->addWidget(create_label("Настройка срабатывания ПКУ (1-64)"));
    main_layout->addWidget(create_label("Измеренная длительность импульсов ПКУ"));

    // Одно большое текстовое поле вывода (куда пишется вся информация)
    QTextEdit *pku_log = create_text_edit(central_widget);
    pku_log->setReadOnly(true);
    main_layout->addWidget(pku_log);

    // Строка системного статуса
    main_layout->addWidget(create_label("Статус ПКУ: устройство готово к работе"));

    // === БЛОК: СТАТУС И СВЯЗЬ ===
    main_layout->addWidget(create_label("Статус и связь"));

    QPushButton *btn_check_conn = create_button("Проверить соединение", central_widget);
    main_layout->addWidget(btn_check_conn);

    QPushButton *btn_get_status = create_button("Статус оборудования", central_widget);
    main_layout->addWidget(btn_get_status);

    QPushButton *btn_get_version = create_button("Проверить версию", central_widget);
    main_layout->addWidget(btn_get_version);

    // === БЛОК: ОСНОВНАЯ ИНФОРМАЦИЯ ===
    main_layout->addWidget(create_label("Основная информация"));

    QPushButton *btn_read_main = create_button("Читать основную информацию", central_widget);
    main_layout->addWidget(btn_read_main);

    QPushButton *btn_write_main = create_button("Записать основную информацию", central_widget);
    main_layout->addWidget(btn_write_main);

    // === БЛОК: ПАРАМЕТРЫ ПКУ ===
    main_layout->addWidget(create_label("Параметры ПКУ"));

    // Нижняя комбинированная строка с настройками и кнопками
    QHBoxLayout *params_layout = create_h_box_layout();

    QLabel *rk_num_label = create_label("Номер РК");
    QLineEdit *rk_num_input = create_line_edit(central_widget);
    rk_num_input->setMaximumWidth(60);

    QLabel *rk_time_label = create_label("Длительность (мс)");
    QLineEdit *rk_time_input = create_line_edit(central_widget);
    rk_time_input->setMaximumWidth(80);

    QPushButton *btn_send_rk = create_button("Выдать РК", central_widget);

    QLabel *chan_label = create_label("Выбор ПКУ");
    QComboBox *chan_select = create_combo_box(central_widget);
    for (int i = 1; i <= 64; ++i) {
        chan_select->addItem(QString("ПКУ %1").arg(i));
    }

    QLabel *mode_label = create_label("Режим");
    QComboBox *mode_select = create_combo_box(central_widget);
    mode_select->addItem("0 — Отрицательный импульс");
    mode_select->addItem("1 — Положительный импульс");
    mode_select->addItem("2 — Фронт");

    QPushButton *btn_set_mode = create_button("Применить режим", central_widget);

    params_layout->addWidget(rk_num_label);
    params_layout->addWidget(rk_num_input);
    params_layout->addWidget(rk_time_label);
    params_layout->addWidget(rk_time_input);
    params_layout->addWidget(btn_send_rk);
    params_layout->addWidget(chan_label);
    params_layout->addWidget(chan_select);
    params_layout->addWidget(mode_label);
    params_layout->addWidget(mode_select);
    params_layout->addWidget(btn_set_mode);
    main_layout->addLayout(params_layout);

    // Самая нижняя кнопка
    QPushButton *btn_read_pku = create_button("Читать длительности ПКУ", central_widget);
    main_layout->addWidget(btn_read_pku);

    central_widget->setLayout(main_layout);
}

void QtViewPKU::show() {
    if (stacked_widget->indexOf(central_widget) == NOT_FOUND) {
        stacked_widget->addWidget(central_widget);
    }
    stacked_widget->setCurrentWidget(central_widget);
}

// =========================================================================
// БЕЗОПАСНЫЕ ФУНКЦИИ ВЫДЕЛЕНИЯ ПАМЯТИ
// =========================================================================

QWidget *QtViewPKU::create_widget() const {
    QWidget *widget{nullptr};
    try { widget = new QWidget{}; }
    catch (const std::bad_alloc &) { std::cerr << "Не удалось создать QWidget." << std::endl; }
    return widget;
}

QVBoxLayout *QtViewPKU::create_v_box_layout(QWidget *const parent) const {
    QVBoxLayout *layout{nullptr};
    try { layout = new QVBoxLayout{parent}; }
    catch (const std::bad_alloc &) { std::cerr << "Не удалось создать QVBoxLayout." << std::endl; }
    return layout;
}

QHBoxLayout *QtViewPKU::create_h_box_layout() const {
    QHBoxLayout *layout{nullptr};
    try { layout = new QHBoxLayout{}; }
    catch (const std::bad_alloc &) { std::cerr << "Не удалось создать QHBoxLayout." << std::endl; }
    return layout;
}

QLabel *QtViewPKU::create_label(const QString &text) const {
    QLabel *label{nullptr};
    try { label = new QLabel{text}; }
    catch (const std::bad_alloc &) { std::cerr << "Не удалось создать QLabel." << std::endl; }
    return label;
}

QLineEdit *QtViewPKU::create_line_edit(QWidget *const parent) const {
    QLineEdit *line_edit{nullptr};
    try { line_edit = new QLineEdit{parent}; }
    catch (const std::bad_alloc &) { std::cerr << "Не удалось создать QLineEdit." << std::endl; }
    return line_edit;
}

QComboBox *QtViewPKU::create_combo_box(QWidget *const parent) const {
    QComboBox *combo_box{nullptr};
    try { combo_box = new QComboBox{parent}; }
    catch (const std::bad_alloc &) { std::cerr << "Не удалось создать QComboBox." << std::endl; }
    return combo_box;
}

QTextEdit *QtViewPKU::create_text_edit(QWidget *const parent) const {
    QTextEdit *text_edit{nullptr};
    try { text_edit = new QTextEdit{parent}; }
    catch (const std::bad_alloc &) { std::cerr << "Не удалось создать QTextEdit." << std::endl; }
    return text_edit;
}

QPushButton *QtViewPKU::create_button(const QString &text, QWidget *const parent) const {
    QPushButton *button{nullptr};
    try { button = new QPushButton{text, parent}; }
    catch (const std::bad_alloc &) { std::cerr << "Не удалось создать QPushButton." << std::endl; }
    return button;
}


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

    // --- СЕКЦИЯ: СТАТУС И СВЯЗЬ ---
    main_layout->addWidget(create_label("<b>Статус и связь</b>"));

    // 1. Строка с кнопками для Статуса и Связи
    QHBoxLayout *status_buttons_layout = create_h_box_layout();

    QPushButton *btn_check_conn = create_button("Проверить соединение", central_widget);
    btn_check_conn->setFixedWidth(200); // Задали одинаковую ширину для всех трех кнопок
    status_buttons_layout->addWidget(btn_check_conn);

    QPushButton *btn_get_status = create_button("Статус оборудования", central_widget);
    btn_get_status->setFixedWidth(200);
    status_buttons_layout->addWidget(btn_get_status);

    QPushButton *btn_get_version = create_button("Проверить версию", central_widget);
    btn_get_version->setFixedWidth(200);
    status_buttons_layout->addWidget(btn_get_version);

    status_buttons_layout->addStretch(); // Сдвигаем всё влево
    main_layout->addLayout(status_buttons_layout);

    // 2. Строка со статусами (строго ПОД кнопками)
    QHBoxLayout *status_labels_layout = create_h_box_layout();

    QLabel *lbl_connect_status = create_label("Статус: не проверено");
    lbl_connect_status->setFixedWidth(200); // Ограничили ширину, чтобы статус стоял точно под своей кнопкой
    status_labels_layout->addWidget(lbl_connect_status);

    QLabel *lbl_equipment_status = create_label("Статус: нет данных");
    lbl_equipment_status->setFixedWidth(200);
    status_labels_layout->addWidget(lbl_equipment_status);

    QLabel *lbl_version_status = create_label("Версия: --");
    lbl_version_status->setFixedWidth(200);
    status_labels_layout->addWidget(lbl_version_status);

    status_labels_layout->addStretch();
    main_layout->addLayout(status_labels_layout);

    main_layout->addSpacing(15);

    // --- СЕКЦИЯ: ОСНОВНАЯ ИНФОРМАЦИЯ ---
    main_layout->addWidget(create_label("<b>Основная информация</b>"));

    // 3. Строка с кнопками для Основной Информации
    QHBoxLayout *main_info_buttons_layout = create_h_box_layout();

    QPushButton *btn_read_main = create_button("Читать основную информацию", central_widget);
    btn_read_main->setFixedWidth(250); // Задали одинаковую ширину для двух кнопок
    main_info_buttons_layout->addWidget(btn_read_main);

    QPushButton *btn_write_main = create_button("Записать основную информацию", central_widget);
    btn_write_main->setFixedWidth(250);
    main_info_buttons_layout->addWidget(btn_write_main);

    main_info_buttons_layout->addStretch();
    main_layout->addLayout(main_info_buttons_layout);

    // 4. Строка со статусами (строго ПОД кнопками основной информации)
    QHBoxLayout *main_info_labels_layout = create_h_box_layout();

    QLabel *lbl_read_main_status = create_label("Данные не прочитаны");
    lbl_read_main_status->setFixedWidth(250); // Текст встанет ровно под своей кнопкой
    main_info_labels_layout->addWidget(lbl_read_main_status);

    QLabel *lbl_write_main_status = create_label("Изменения не записаны");
    lbl_write_main_status->setFixedWidth(250);
    main_info_labels_layout->addWidget(lbl_write_main_status);

    main_info_labels_layout->addStretch();
    main_layout->addLayout(main_info_labels_layout);

    main_layout->addSpacing(15);
    main_layout->addSpacing(15);

    // --- СЕКЦИЯ ПОДГОТОВКИ ЭЛЕМЕНТОВ ДЛЯ НИЖНИХ БЛОКОВ ---
    QLabel *rk_num_label = create_label("Номер РК");
    QLineEdit *rk_num_input = create_line_edit(central_widget);

    QLabel *rk_time_label = create_label("Длительность (мс)");
    QLineEdit *rk_time_input = create_line_edit(central_widget);

    QPushButton *btn_send_rk = create_button("Выдать РК", central_widget);

    QLabel *chan_label = create_label("Выбор ПКУ");
    QComboBox *chan_select = create_combo_box(central_widget);
    for (int i = 1; i <= 64; ++i) {
        chan_select->addItem(QString("ПКУ %1").arg(i));
    }

    QLabel *mode_label = create_label("Режим");
    QComboBox *mode_select = create_combo_box(central_widget);
    mode_select->addItem("0 - Отрицательный импульс");
    mode_select->addItem("1 - Положительный импульс");
    mode_select->addItem("2 - Фронт");

    QPushButton *btn_set_mode = create_button("Применить режим", central_widget);

    // === БЛОК КОМАНДЫ 1: ВЫДАЧА РК ===
    main_layout->addWidget(create_label("<b>Выдача РК</b>"));

    QHBoxLayout *rk_fields_layout = create_h_box_layout();
    rk_fields_layout->addWidget(rk_num_label);
    rk_num_input->setMaximumWidth(60);
    rk_fields_layout->addWidget(rk_num_input);

    rk_fields_layout->addWidget(rk_time_label);
    rk_time_input->setMaximumWidth(80);
    rk_fields_layout->addWidget(rk_time_input);

    btn_send_rk->setMaximumWidth(150);
    rk_fields_layout->addWidget(btn_send_rk);
    rk_fields_layout->addStretch();
    main_layout->addLayout(rk_fields_layout);

    QLabel *lbl_send_rk_status = create_label("Команда выдачи РК не отправлялась");
    main_layout->addWidget(lbl_send_rk_status);

    main_layout->addSpacing(15);
    // === БЛОК КОМАНДЫ 2: ПАРАМЕТРЫ ПКУ ===
    main_layout->addWidget(create_label("<b>Измеренная длительность импульсов ПКУ</b>"));

    QHBoxLayout *pku_fields_layout = create_h_box_layout();
    pku_fields_layout->addWidget(chan_label);
    chan_select->setMaximumWidth(120);
    pku_fields_layout->addWidget(chan_select);

    pku_fields_layout->addWidget(mode_label);
    mode_select->setMaximumWidth(220);
    pku_fields_layout->addWidget(mode_select);

    btn_set_mode->setMaximumWidth(150);
    pku_fields_layout->addWidget(btn_set_mode);
    pku_fields_layout->addStretch();
    main_layout->addLayout(pku_fields_layout);

    QLabel *lbl_set_mode_status = create_label("Режим не применялся");
    main_layout->addWidget(lbl_set_mode_status);

    main_layout->addSpacing(15);

    // === БЛОК КОМАНДЫ 3: ЧТЕНИЕ ДЛИТЕЛЬНОСТЕЙ ПКУ ===

    // === БЛОК КОМАНДЫ 3: ЧТЕНИЕ ДЛИТЕЛЬНОСТЕЙ ПКУ ===
    main_layout->addWidget(create_label("<b>Чтение длительностей ПКУ</b>"));

    QHBoxLayout *read_pku_layout = create_h_box_layout();

    QLabel *lbl_multi_pku = create_label("Выбор номеров ПКУ:");
    read_pku_layout->addWidget(lbl_multi_pku);

    QComboBox *multi_pku_select = create_multi_select_combo_box(central_widget, 64);
    multi_pku_select->setFixedWidth(250);
    read_pku_layout->addWidget(multi_pku_select);

    // Добавили QPushButton* перед переменной
    QPushButton *btn_read_pku = create_button("Читать длительности ПКУ", central_widget);
    btn_read_pku->setFixedWidth(220);
    read_pku_layout->addWidget(btn_read_pku);

    read_pku_layout->addStretch();
    main_layout->addLayout(read_pku_layout);

    QLabel *lbl_read_pku_status = create_label("Длительности ПКУ не считывались");
    main_layout->addWidget(lbl_read_pku_status);

    // Добавили QTextEdit* перед переменной
    QTextEdit *pku_log = create_text_edit(central_widget);
    pku_log->setReadOnly(true);
    pku_log->setMinimumHeight(180);
    main_layout->addWidget(pku_log);

    central_widget->setLayout(main_layout);
} // <--- ЭТА СКОБКА ТЕПЕРЬ НА МЕСТЕ, ОНА ЗАКРЫВАЕТ КОНСТРУКТОР КЛАССА

void QtViewPKU::show() {
    if (stacked_widget->indexOf(central_widget) == NOT_FOUND) {
        stacked_widget->addWidget(central_widget);
    }
    stacked_widget->setCurrentWidget(central_widget);
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

QComboBox *QtViewPKU::create_multi_select_combo_box(QWidget *const parent, int count) const {
    QComboBox *combo_box{nullptr};
    try {
        combo_box = new QComboBox{parent};
        // Если компилятор ругнётся на QStandardItemModel, вашему другу нужно будет добавить
        // строку #include <QStandardItemModel> в самый верх этого файла или в qt_view_pku.h
        QStandardItemModel *model = new QStandardItemModel(count, 1, combo_box);
        for (int i = 0; i < count; ++i) {
            QStandardItem *item = new QStandardItem(QString("ПКУ %1").arg(i + 1));
            item->setFlags(Qt::ItemIsUserCheckable | Qt::ItemIsEnabled);
            item->setData(Qt::Unchecked, Qt::CheckStateRole);
            model->setItem(i, 0, item);
        }
        combo_box->setModel(model);
    }
    catch (const std::bad_alloc &) {
        std::cerr << "Не удалось создать множественный QComboBox." << std::endl;
    }
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

QWidget *QtViewPKU::create_widget() const {
    QWidget *widget{nullptr};
    try { widget = new QWidget{}; }
    catch (const std::bad_alloc &) { std::cerr << "Не удалось создать QWidget." << std::endl; }
    return widget;
}

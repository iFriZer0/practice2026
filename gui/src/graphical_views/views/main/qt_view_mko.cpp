#include <iostream>
#include <exception>
#include <limits>
#include <memory>
#include <new>
#include <QComboBox>
#include <QDateTime>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QScrollArea>
#include <QSpinBox>
#include <QStackedWidget>
#include <QString>
#include <QStringList>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include "grpc_mko_client.h"
#include "qt_view_mko.h"

QtViewMKO::QtViewMKO(QStackedWidget *const stacked_widget) noexcept
    : central_widget{nullptr},
      stacked_widget{stacked_widget},
      operation_log{nullptr},
      mko_client{nullptr}
{
    try
    {
        mko_client = std::make_shared<GrpcMkoClient>(DEFAULT_MKO_SERVICE_ADDRESS);
    }
    catch (const std::exception &exception)
    {
        std::cerr << "Не удалось создать gRPC-клиент МКО: " << exception.what() << std::endl;
    }

    central_widget = create_widget();

    QVBoxLayout *main_layout{create_v_box_layout(central_widget)};
    main_layout->addWidget(create_label("МКО"));

    QHBoxLayout *work_area_layout{new QHBoxLayout{}};
    work_area_layout->addWidget(create_kk_panel());
    work_area_layout->addWidget(create_ou_panel());
    main_layout->addLayout(work_area_layout);

    main_layout->addWidget(create_log_panel());
    central_widget->setLayout(main_layout);
}

void QtViewMKO::show()
{
    if (stacked_widget->indexOf(central_widget) == NOT_FOUND)
    {
        stacked_widget->addWidget(central_widget);
    }
    stacked_widget->setCurrentWidget(central_widget);
}

QWidget *QtViewMKO::create_widget() const
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

QVBoxLayout *QtViewMKO::create_v_box_layout(QWidget *const parent) const
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

QLabel *QtViewMKO::create_label(const QString &text) const
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

QWidget *QtViewMKO::create_kk_panel()
{
    QScrollArea *scroll_area{new QScrollArea{central_widget}};
    scroll_area->setWidgetResizable(true);

    QWidget *panel{new QWidget{scroll_area}};
    QVBoxLayout *panel_layout{new QVBoxLayout{panel}};

    QGroupBox *configure_group{create_group_box("КК: ConfigureKK")};
    QGridLayout *configure_layout{new QGridLayout{configure_group}};

    QLineEdit *board_id{create_line_edit("board-1")};
    QSpinBox *index{create_spin_box(1, 2, 1)};
    QComboBox *channel{create_channel_combo_box()};
    QSpinBox *bus_control{create_spin_box(0, 1, 0)};
    QSpinBox *ou_address{create_spin_box(0, 30, 1)};
    QSpinBox *ou_resp_word{create_spin_box(0, std::numeric_limits<unsigned short>::max(), 0)};
    QSpinBox *vector_word{create_spin_box(0, std::numeric_limits<unsigned short>::max(), 0)};
    QSpinBox *selftest_word{create_spin_box(0, std::numeric_limits<unsigned short>::max(), 0)};
    QLineEdit *remote_ip{create_line_edit("192.168.2.254")};
    QSpinBox *remote_port{create_spin_box(1, std::numeric_limits<unsigned short>::max(), 4000)};
    QLineEdit *configure_operation_id{create_line_edit("configure-kk-1")};

    add_labeled_widget(configure_layout, 0, "Board ID", board_id);
    add_labeled_widget(configure_layout, 1, "МКО", index);
    add_labeled_widget(configure_layout, 2, "Канал", channel);
    add_labeled_widget(configure_layout, 3, "Bus control", bus_control);
    add_labeled_widget(configure_layout, 4, "Адрес ОУ", ou_address);
    add_labeled_widget(configure_layout, 5, "Ответное слово ОУ", ou_resp_word);
    add_labeled_widget(configure_layout, 6, "Векторное слово", vector_word);
    add_labeled_widget(configure_layout, 7, "Слово самотеста", selftest_word);
    add_labeled_widget(configure_layout, 8, "Remote IP", remote_ip);
    add_labeled_widget(configure_layout, 9, "Remote port", remote_port);
    add_labeled_widget(configure_layout, 10, "Operation ID", configure_operation_id);

    QPushButton *configure_button{new QPushButton{"Настроить КК", configure_group}};
    configure_layout->addWidget(configure_button, 11, 0, 1, 2);

    QObject::connect(configure_button, &QPushButton::clicked, [this,
                                                               board_id,
                                                               index,
                                                               channel,
                                                               bus_control,
                                                               ou_address,
                                                               ou_resp_word,
                                                               vector_word,
                                                               selftest_word,
                                                               remote_ip,
                                                               remote_port,
                                                               configure_operation_id]() {
        if (mko_client == nullptr)
        {
            append_log("ConfigureKK: gRPC-клиент МКО не создан.");
            return;
        }

        ConfigureKKRequestData request{
                board_id->text().toStdString(),
                index->value(),
                channel->currentData().toInt(),
                bus_control->value(),
                ou_address->value(),
                ou_resp_word->value(),
                vector_word->value(),
                selftest_word->value(),
                remote_ip->text().toStdString(),
                remote_port->value(),
                configure_operation_id->text().toStdString()
        };

        try
        {
            mko_client->configure_kk(request);
            append_log(
                    QString{"ConfigureKK: успешно board=%1 index=%2 channel=%3 ou=%4 remote=%5:%6"}
                            .arg(board_id->text())
                            .arg(index->value())
                            .arg(channel->currentData().toInt())
                            .arg(ou_address->value())
                            .arg(remote_ip->text())
                            .arg(remote_port->value())
            );
        }
        catch (const std::exception &exception)
        {
            append_log(QString{"ConfigureKK: ошибка: %1"}.arg(exception.what()));
        }
    });

    QGroupBox *exchange_group{create_group_box("КК: ConfigureExchange")};
    QGridLayout *exchange_layout{new QGridLayout{exchange_group}};

    QSpinBox *format{create_spin_box(1, 10, 1)};
    QSpinBox *ks1{create_spin_box(0, std::numeric_limits<unsigned short>::max(), 0)};
    QSpinBox *ks2{create_spin_box(0, std::numeric_limits<unsigned short>::max(), 0)};
    QLineEdit *exchange_operation_id{create_line_edit("exchange-1")};
    QTableWidget *kk_words{create_words_table()};

    add_labeled_widget(exchange_layout, 0, "Формат", format);
    add_labeled_widget(exchange_layout, 1, "КС1", ks1);
    add_labeled_widget(exchange_layout, 2, "КС2", ks2);
    add_labeled_widget(exchange_layout, 3, "Operation ID", exchange_operation_id);
    exchange_layout->addWidget(create_label("Слова данных СД1-СД32"), 4, 0, 1, 2);
    exchange_layout->addWidget(kk_words, 5, 0, 1, 2);

    QPushButton *exchange_button{new QPushButton{"Настроить обмен", exchange_group}};
    QPushButton *run_button{new QPushButton{"RUN: запустить обмен", exchange_group}};
    QPushButton *subscribe_button{new QPushButton{"Подписаться на результаты", exchange_group}};

    exchange_layout->addWidget(exchange_button, 6, 0, 1, 2);
    exchange_layout->addWidget(run_button, 7, 0, 1, 2);
    exchange_layout->addWidget(subscribe_button, 8, 0, 1, 2);

    QObject::connect(exchange_button, &QPushButton::clicked, [this,
                                                              board_id,
                                                              format,
                                                              ks1,
                                                              ks2,
                                                              kk_words,
                                                              exchange_operation_id]() {
        if (mko_client == nullptr)
        {
            append_log("ConfigureExchange: gRPC-клиент МКО не создан.");
            return;
        }

        ConfigureExchangeRequestData request{
                board_id->text().toStdString(),
                format->value(),
                static_cast<uint32_t>(ks1->value()),
                static_cast<uint32_t>(ks2->value()),
                collect_words(kk_words),
                exchange_operation_id->text().toStdString()
        };

        try
        {
            mko_client->configure_exchange(request);
            append_log(
                    QString{"ConfigureExchange: успешно board=%1 format=%2 ks1=%3 ks2=%4"}
                            .arg(board_id->text())
                            .arg(format->value())
                            .arg(ks1->value())
                            .arg(ks2->value())
            );
        }
        catch (const std::exception &exception)
        {
            append_log(QString{"ConfigureExchange: ошибка: %1"}.arg(exception.what()));
        }
    });
    QObject::connect(run_button, &QPushButton::clicked, [this, board_id, exchange_operation_id]() {
        if (mko_client == nullptr)
        {
            append_log("RunExchange: gRPC-клиент МКО не создан.");
            return;
        }

        RunExchangeRequestData request{
                board_id->text().toStdString(),
                exchange_operation_id->text().toStdString()
        };

        try
        {
            mko_client->run_exchange(request);
            append_log(
                    QString{"RunExchange: успешно board=%1 operation_id=%2"}
                            .arg(board_id->text())
                            .arg(exchange_operation_id->text())
            );
        }
        catch (const std::exception &exception)
        {
            append_log(QString{"RunExchange: ошибка: %1"}.arg(exception.what()));
        }
    });
    QObject::connect(subscribe_button, &QPushButton::clicked, [this, board_id]() {
        append_log(QString{"SubscribeExchangeResults: board=%1"}.arg(board_id->text()));
    });

    panel_layout->addWidget(configure_group);
    panel_layout->addWidget(exchange_group);
    panel_layout->addStretch();

    scroll_area->setWidget(panel);
    return scroll_area;
}

QWidget *QtViewMKO::create_ou_panel()
{
    QScrollArea *scroll_area{new QScrollArea{central_widget}};
    scroll_area->setWidgetResizable(true);

    QWidget *panel{new QWidget{scroll_area}};
    QVBoxLayout *panel_layout{new QVBoxLayout{panel}};

    QGroupBox *configure_group{create_group_box("ОУ: ConfigureOU")};
    QGridLayout *configure_layout{new QGridLayout{configure_group}};

    QLineEdit *board_id{create_line_edit("board-1")};
    QSpinBox *index{create_spin_box(1, 2, 1)};
    QComboBox *channel{create_channel_combo_box()};
    QSpinBox *ou_address{create_spin_box(0, 30, 1)};
    QSpinBox *response_word{create_spin_box(0, std::numeric_limits<unsigned short>::max(), 0)};
    QLineEdit *remote_ip{create_line_edit("192.168.2.254")};
    QSpinBox *remote_port{create_spin_box(1, std::numeric_limits<unsigned short>::max(), 4000)};

    add_labeled_widget(configure_layout, 0, "Board ID", board_id);
    add_labeled_widget(configure_layout, 1, "МКО", index);
    add_labeled_widget(configure_layout, 2, "Канал", channel);
    add_labeled_widget(configure_layout, 3, "Адрес ОУ", ou_address);
    add_labeled_widget(configure_layout, 4, "Ответное слово", response_word);
    add_labeled_widget(configure_layout, 5, "Remote IP", remote_ip);
    add_labeled_widget(configure_layout, 6, "Remote port", remote_port);

    QPushButton *configure_button{new QPushButton{"Настроить ОУ", configure_group}};
    configure_layout->addWidget(configure_button, 7, 0, 1, 2);

    QObject::connect(configure_button, &QPushButton::clicked, [this, board_id, index, channel, ou_address, remote_ip, remote_port]() {
        append_log(
                QString{"ConfigureOU: board=%1 index=%2 channel=%3 ou=%4 remote=%5:%6"}
                        .arg(board_id->text())
                        .arg(index->value())
                        .arg(channel->currentData().toInt())
                        .arg(ou_address->value())
                        .arg(remote_ip->text())
                        .arg(remote_port->value())
        );
    });

    QGroupBox *subaddress_group{create_group_box("ОУ: подадреса и команды")};
    QGridLayout *subaddress_layout{new QGridLayout{subaddress_group}};

    QSpinBox *subaddress{create_spin_box(0, 30, 0)};
    QComboBox *area{new QComboBox{subaddress_group}};
    area->addItem("Область приема", true);
    area->addItem("Область передачи", false);
    QTableWidget *ou_words{create_words_table()};

    add_labeled_widget(subaddress_layout, 0, "Подадрес", subaddress);
    add_labeled_widget(subaddress_layout, 1, "Область", area);
    subaddress_layout->addWidget(create_label("Слова данных СД1-СД32"), 2, 0, 1, 2);
    subaddress_layout->addWidget(ou_words, 3, 0, 1, 2);

    QPushButton *read_button{new QPushButton{"Прочитать подадрес", subaddress_group}};
    QPushButton *write_button{new QPushButton{"Записать подадрес", subaddress_group}};
    QPushButton *set_response_word_button{new QPushButton{"Установить ответное слово", subaddress_group}};
    QPushButton *subscribe_button{new QPushButton{"Подписаться на команды ОУ", subaddress_group}};

    subaddress_layout->addWidget(read_button, 4, 0, 1, 2);
    subaddress_layout->addWidget(write_button, 5, 0, 1, 2);
    subaddress_layout->addWidget(set_response_word_button, 6, 0, 1, 2);
    subaddress_layout->addWidget(subscribe_button, 7, 0, 1, 2);

    QObject::connect(read_button, &QPushButton::clicked, [this, subaddress, area]() {
        append_log(
                QString{"ReadOuSubaddress: subaddress=%1 receive_area=%2"}
                        .arg(subaddress->value())
                        .arg(area->currentData().toBool() ? "true" : "false")
        );
    });
    QObject::connect(write_button, &QPushButton::clicked, [this, subaddress]() {
        append_log(QString{"WriteOuSubaddress: subaddress=%1"}.arg(subaddress->value()));
    });
    QObject::connect(set_response_word_button, &QPushButton::clicked, [this, response_word]() {
        append_log(QString{"SetOuResponseWord: response_word=%1"}.arg(response_word->value()));
    });
    QObject::connect(subscribe_button, &QPushButton::clicked, [this, board_id]() {
        append_log(QString{"SubscribeOuCommands: board=%1"}.arg(board_id->text()));
    });

    panel_layout->addWidget(configure_group);
    panel_layout->addWidget(subaddress_group);
    panel_layout->addStretch();

    scroll_area->setWidget(panel);
    return scroll_area;
}

QWidget *QtViewMKO::create_log_panel()
{
    QGroupBox *group{create_group_box("Журнал операций")};
    QVBoxLayout *layout{new QVBoxLayout{group}};

    operation_log = new QTextEdit{group};
    operation_log->setReadOnly(true);
    operation_log->setMinimumHeight(130);

    layout->addWidget(operation_log);
    append_log(QString{"Вкладка МКО готова. gRPC endpoint: %1"}.arg(DEFAULT_MKO_SERVICE_ADDRESS));

    return group;
}

QGroupBox *QtViewMKO::create_group_box(const QString &title) const
{
    QGroupBox *group{nullptr};
    try
    {
        group = new QGroupBox{title};
    }
    catch (const std::bad_alloc &)
    {
        std::cerr << "Не удалось создать QGroupBox." << std::endl;
    }
    return group;
}

QLineEdit *QtViewMKO::create_line_edit(const QString &placeholder) const
{
    QLineEdit *line_edit{nullptr};
    try
    {
        line_edit = new QLineEdit{};
        line_edit->setText(placeholder);
    }
    catch (const std::bad_alloc &)
    {
        std::cerr << "Не удалось создать QLineEdit." << std::endl;
    }
    return line_edit;
}

QSpinBox *QtViewMKO::create_spin_box(int minimum, int maximum, int value) const
{
    QSpinBox *spin_box{nullptr};
    try
    {
        spin_box = new QSpinBox{};
        spin_box->setRange(minimum, maximum);
        spin_box->setValue(value);
    }
    catch (const std::bad_alloc &)
    {
        std::cerr << "Не удалось создать QSpinBox." << std::endl;
    }
    return spin_box;
}

QComboBox *QtViewMKO::create_channel_combo_box() const
{
    QComboBox *combo_box{nullptr};
    try
    {
        combo_box = new QComboBox{};
        combo_box->addItem("Основной", 0);
        combo_box->addItem("Резервный", 1);
    }
    catch (const std::bad_alloc &)
    {
        std::cerr << "Не удалось создать QComboBox." << std::endl;
    }
    return combo_box;
}

QTableWidget *QtViewMKO::create_words_table() const
{
    QTableWidget *table{nullptr};
    try
    {
        table = new QTableWidget{WORD_COUNT, 2};
        table->setHorizontalHeaderLabels(QStringList{"Слово", "Значение"});
        table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
        table->verticalHeader()->setVisible(false);
        table->setMinimumHeight(260);

        for (int row{0}; row < WORD_COUNT; ++row)
        {
            QTableWidgetItem *name_item{new QTableWidgetItem{QString{"СД%1"}.arg(row + 1)}};
            name_item->setFlags(name_item->flags() & ~Qt::ItemIsEditable);
            table->setItem(row, 0, name_item);
            table->setItem(row, 1, new QTableWidgetItem{"0"});
        }
    }
    catch (const std::bad_alloc &)
    {
        std::cerr << "Не удалось создать QTableWidget." << std::endl;
    }
    return table;
}

void QtViewMKO::add_labeled_widget(QGridLayout *const layout, int row, const QString &label_text, QWidget *const widget) const
{
    layout->addWidget(create_label(label_text), row, 0);
    layout->addWidget(widget, row, 1);
}

std::vector<uint32_t> QtViewMKO::collect_words(const QTableWidget *const table) const
{
    std::vector<uint32_t> words;
    words.reserve(WORD_COUNT);

    for (int row{0}; row < WORD_COUNT; ++row)
    {
        const QTableWidgetItem *item{table->item(row, 1)};
        bool is_valid{false};
        const uint32_t value{item == nullptr ? 0U : item->text().toUInt(&is_valid, 0)};
        words.push_back(is_valid ? value : 0U);
    }

    return words;
}

void QtViewMKO::append_log(const QString &message)
{
    if (operation_log == nullptr)
    {
        return;
    }

    operation_log->append(
            QString{"[%1] %2"}
                    .arg(QDateTime::currentDateTime().toString("HH:mm:ss"))
                    .arg(message)
    );
}

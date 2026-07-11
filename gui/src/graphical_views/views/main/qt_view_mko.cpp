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
#include <QMetaObject>
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
      kk_index_spin_box{nullptr},
      ou_index_spin_box{nullptr},
      mko_client{nullptr},
      is_updating_mko_indices{false}
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
    main_layout->addWidget(create_menu_panel());

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
    kk_index_spin_box = index;
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

    QGroupBox *configure_group{create_group_box("ОУ: ConfigureOu")};
    QGridLayout *configure_layout{new QGridLayout{configure_group}};

    QLineEdit *configure_board_id{create_line_edit("board-1")};
    QSpinBox *configure_index{create_spin_box(1, 2, 2)};
    ou_index_spin_box = configure_index;
    QComboBox *configure_channel{create_channel_combo_box()};
    QSpinBox *configure_ou_address{create_spin_box(0, 30, 1)};
    QSpinBox *configure_response_word{create_spin_box(0, std::numeric_limits<unsigned short>::max(), 0)};
    QLineEdit *configure_remote_ip{create_line_edit("192.168.2.254")};
    QSpinBox *configure_remote_port{create_spin_box(1, std::numeric_limits<unsigned short>::max(), 4000)};

    add_labeled_widget(configure_layout, 0, "Board ID", configure_board_id);
    add_labeled_widget(configure_layout, 1, "МКО", configure_index);
    add_labeled_widget(configure_layout, 2, "Канал", configure_channel);
    add_labeled_widget(configure_layout, 3, "Адрес ОУ", configure_ou_address);
    add_labeled_widget(configure_layout, 4, "Ответное слово", configure_response_word);
    add_labeled_widget(configure_layout, 5, "Remote IP", configure_remote_ip);
    add_labeled_widget(configure_layout, 6, "Remote port", configure_remote_port);

    QPushButton *configure_button{new QPushButton{"Настроить ОУ", configure_group}};
    configure_layout->addWidget(configure_button, 7, 0, 1, 2);

    QObject::connect(kk_index_spin_box, QOverload<int>::of(&QSpinBox::valueChanged), [this]() {
        sync_mko_indices(kk_index_spin_box);
    });
    QObject::connect(ou_index_spin_box, QOverload<int>::of(&QSpinBox::valueChanged), [this]() {
        sync_mko_indices(ou_index_spin_box);
    });

    QObject::connect(configure_button, &QPushButton::clicked, [this,
                                                               configure_board_id,
                                                               configure_index,
                                                               configure_channel,
                                                               configure_ou_address,
                                                               configure_response_word,
                                                               configure_remote_ip,
                                                               configure_remote_port]() {
        if (mko_client == nullptr)
        {
            append_log("ConfigureOu: gRPC-клиент МКО не создан.");
            return;
        }

        ConfigureOuRequestData request{
                configure_board_id->text().toStdString(),
                configure_index->value(),
                configure_channel->currentData().toInt(),
                configure_ou_address->value(),
                configure_response_word->value(),
                configure_remote_ip->text().toStdString(),
                configure_remote_port->value()
        };

        try
        {
            mko_client->configure_ou(request);
            append_log(
                    QString{"ConfigureOu: успешно board=%1 index=%2 channel=%3 ou=%4 response_word=%5 remote=%6:%7"}
                            .arg(configure_board_id->text())
                            .arg(configure_index->value())
                            .arg(configure_channel->currentData().toInt())
                            .arg(configure_ou_address->value())
                            .arg(configure_response_word->value())
                            .arg(configure_remote_ip->text())
                            .arg(configure_remote_port->value())
            );
        }
        catch (const std::exception &exception)
        {
            append_log(QString{"ConfigureOu: ошибка: %1"}.arg(exception.what()));
        }
    });

    QGroupBox *subscribe_group{create_group_box("ОУ: SubscribeOuCommands")};
    QGridLayout *subscribe_layout{new QGridLayout{subscribe_group}};

    QLineEdit *subscribe_board_id{create_line_edit("board-1")};
    add_labeled_widget(subscribe_layout, 0, "Board ID", subscribe_board_id);

    QPushButton *subscribe_button{new QPushButton{"Подписаться на команды ОУ", subscribe_group}};
    subscribe_button->setProperty("subscribed", false);
    subscribe_layout->addWidget(subscribe_button, 1, 0, 1, 2);

    // SubscribeOuCommands — server-streaming RPC: подписка открывает
    // фоновый поток в GrpcMkoClient, события приходят не в GUI-потоке,
    // поэтому обновление виджетов делается через
    // QMetaObject::invokeMethod(..., Qt::QueuedConnection) с
    // central_widget в роли контекста — это и переносит вызов в поток
    // GUI, и гарантирует, что колбэк не выстрелит после уничтожения
    // виджета.
    QObject::connect(subscribe_button, &QPushButton::clicked, [this, subscribe_button, subscribe_board_id]() {
        if (mko_client == nullptr)
        {
            append_log("SubscribeOuCommands: gRPC-клиент МКО не создан.");
            return;
        }

        const bool is_subscribed{subscribe_button->property("subscribed").toBool()};
        if (is_subscribed)
        {
            mko_client->unsubscribe_ou_commands();
            subscribe_button->setProperty("subscribed", false);
            subscribe_button->setText("Подписаться на команды ОУ");
            subscribe_board_id->setEnabled(true);
            append_log(QString{"SubscribeOuCommands: отписка board=%1"}.arg(subscribe_board_id->text()));
            return;
        }

        const SubscribeOuCommandsRequestData request{subscribe_board_id->text().toStdString()};
        QWidget *const ui_context{central_widget};

        try
        {
            mko_client->subscribe_ou_commands(
                    request,
                    [this, ui_context](const OuCommandEventData &event) {
                        QMetaObject::invokeMethod(
                                ui_context,
                                [this, event]() { append_ou_command_event(event); },
                                Qt::QueuedConnection);
                    },
                    [this, ui_context, subscribe_button, subscribe_board_id](const std::string &error_message) {
                        QMetaObject::invokeMethod(
                                ui_context,
                                [this, subscribe_button, subscribe_board_id, error_message]() {
                                    append_log(
                                            QString{"SubscribeOuCommands: поток завершился с ошибкой: %1"}
                                                    .arg(QString::fromStdString(error_message)));
                                    subscribe_button->setProperty("subscribed", false);
                                    subscribe_button->setText("Подписаться на команды ОУ");
                                    subscribe_board_id->setEnabled(true);
                                },
                                Qt::QueuedConnection);
                    });
        }
        catch (const std::exception &exception)
        {
            append_log(QString{"SubscribeOuCommands: ошибка: %1"}.arg(exception.what()));
            return;
        }

        subscribe_button->setProperty("subscribed", true);
        subscribe_button->setText("Отписаться от команд ОУ");
        subscribe_board_id->setEnabled(false);
        append_log(QString{"SubscribeOuCommands: подписка board=%1"}.arg(subscribe_board_id->text()));
    });

    QGroupBox *read_group{create_group_box("ОУ: ReadOuSubaddress")};
    QGridLayout *read_layout{new QGridLayout{read_group}};

    QLineEdit *read_board_id{create_line_edit("board-1")};
    QSpinBox *read_subaddress{create_spin_box(0, 30, 0)};
    QComboBox *read_area{new QComboBox{read_group}};
    read_area->addItem("Область приема", true);
    read_area->addItem("Область передачи", false);
    read_area->setProperty("default_index", 0);

    add_labeled_widget(read_layout, 0, "Board ID", read_board_id);
    add_labeled_widget(read_layout, 1, "Подадрес", read_subaddress);
    add_labeled_widget(read_layout, 2, "Область", read_area);

    QPushButton *read_button{new QPushButton{"Прочитать подадрес", read_group}};
    read_layout->addWidget(read_button, 3, 0, 1, 2);

    QObject::connect(read_button, &QPushButton::clicked, [this, read_board_id, read_subaddress, read_area]() {
        if (mko_client == nullptr)
        {
            append_log("ReadOuSubaddress: gRPC-клиент МКО не создан.");
            return;
        }

        ReadOuSubaddressRequestData request{
                read_board_id->text().toStdString(),
                static_cast<uint32_t>(read_subaddress->value()),
                read_area->currentData().toBool()
        };

        try
        {
            const OuSubaddressData data{mko_client->read_ou_subaddress(request)};
            append_log(
                    QString{"ReadOuSubaddress: успешно board=%1 receive_area=%2"}
                            .arg(read_board_id->text())
                            .arg(read_area->currentData().toBool() ? "true" : "false")
            );
            append_ou_subaddress_data(data);
        }
        catch (const std::exception &exception)
        {
            append_log(QString{"ReadOuSubaddress: ошибка: %1"}.arg(exception.what()));
        }
    });

    QGroupBox *write_group{create_group_box("ОУ: WriteOuSubaddress")};
    QGridLayout *write_layout{new QGridLayout{write_group}};

    QLineEdit *write_board_id{create_line_edit("board-1")};
    QSpinBox *write_subaddress{create_spin_box(0, 30, 0)};
    QTableWidget *write_words{create_words_table()};

    add_labeled_widget(write_layout, 0, "Board ID", write_board_id);
    add_labeled_widget(write_layout, 1, "Подадрес", write_subaddress);
    write_layout->addWidget(create_label("Слова данных СД1-СД32"), 2, 0, 1, 2);
    write_layout->addWidget(write_words, 3, 0, 1, 2);

    QPushButton *write_button{new QPushButton{"Записать подадрес", write_group}};
    write_layout->addWidget(write_button, 4, 0, 1, 2);

    QObject::connect(write_button, &QPushButton::clicked, [this, write_board_id, write_subaddress, write_words]() {
        if (mko_client == nullptr)
        {
            append_log("WriteOuSubaddress: gRPC-клиент МКО не создан.");
            return;
        }

        WriteOuSubaddressRequestData request{
                write_board_id->text().toStdString(),
                static_cast<uint32_t>(write_subaddress->value()),
                collect_words(write_words)
        };

        try
        {
            mko_client->write_ou_subaddress(request);
            append_log(
                    QString{"WriteOuSubaddress: успешно board=%1 subaddress=%2 sd=[%3]"}
                            .arg(write_board_id->text())
                            .arg(write_subaddress->value())
                            .arg(format_words(request.sd))
            );
        }
        catch (const std::exception &exception)
        {
            append_log(QString{"WriteOuSubaddress: ошибка: %1"}.arg(exception.what()));
        }
    });

    QGroupBox *send_raw_group{create_group_box("ОУ: SendRawOuData")};
    QGridLayout *send_raw_layout{new QGridLayout{send_raw_group}};

    QLineEdit *send_raw_board_id{create_line_edit("board-1")};
    QSpinBox *send_raw_subaddress{create_spin_box(0, 30, 0)};
    QTableWidget *send_raw_words{create_words_table()};

    add_labeled_widget(send_raw_layout, 0, "Board ID", send_raw_board_id);
    add_labeled_widget(send_raw_layout, 1, "Подадрес", send_raw_subaddress);
    send_raw_layout->addWidget(create_label("Сырые слова данных СД1-СД32"), 2, 0, 1, 2);
    send_raw_layout->addWidget(send_raw_words, 3, 0, 1, 2);

    QPushButton *send_raw_button{new QPushButton{"Отправить сырые данные ОУ", send_raw_group}};
    send_raw_layout->addWidget(send_raw_button, 4, 0, 1, 2);

    QObject::connect(send_raw_button,
                     &QPushButton::clicked,
                     [this, send_raw_board_id, send_raw_subaddress, send_raw_words]() {
        if (mko_client == nullptr)
        {
            append_log("SendRawOuData: gRPC-клиент МКО не создан.");
            return;
        }

        WriteOuSubaddressRequestData request{
                send_raw_board_id->text().toStdString(),
                static_cast<uint32_t>(send_raw_subaddress->value()),
                collect_words(send_raw_words)
        };

        try
        {
            mko_client->send_raw_ou_data(request);
            append_log(
                    QString{"SendRawOuData: успешно board=%1 subaddress=%2 sd=[%3]"}
                            .arg(send_raw_board_id->text())
                            .arg(send_raw_subaddress->value())
                            .arg(format_words(request.sd))
            );
        }
        catch (const std::exception &exception)
        {
            append_log(QString{"SendRawOuData: ошибка: %1"}.arg(exception.what()));
        }
    });

    QGroupBox *set_response_word_group{create_group_box("ОУ: SetOuResponseWord")};
    QGridLayout *set_response_word_layout{new QGridLayout{set_response_word_group}};

    QLineEdit *set_response_word_board_id{create_line_edit("board-1")};
    QSpinBox *set_response_word_index{create_spin_box(1, 2, 2)};
    QComboBox *set_response_word_channel{create_channel_combo_box()};
    QSpinBox *set_response_word_ou_address{create_spin_box(0, 30, 1)};
    QSpinBox *set_response_word{create_spin_box(0, std::numeric_limits<unsigned short>::max(), 0)};

    add_labeled_widget(set_response_word_layout, 0, "Board ID", set_response_word_board_id);
    add_labeled_widget(set_response_word_layout, 1, "МКО", set_response_word_index);
    add_labeled_widget(set_response_word_layout, 2, "Канал", set_response_word_channel);
    add_labeled_widget(set_response_word_layout, 3, "Адрес ОУ", set_response_word_ou_address);
    add_labeled_widget(set_response_word_layout, 4, "Ответное слово", set_response_word);

    QPushButton *set_response_word_button{new QPushButton{"Установить ответное слово", set_response_word_group}};
    set_response_word_layout->addWidget(set_response_word_button, 5, 0, 1, 2);

    QObject::connect(set_response_word_button,
                     &QPushButton::clicked,
                     [this,
                      set_response_word_board_id,
                      set_response_word_index,
                      set_response_word_channel,
                      set_response_word_ou_address,
                      set_response_word]() {
        if (mko_client == nullptr)
        {
            append_log("SetOuResponseWord: gRPC-клиент МКО не создан.");
            return;
        }

        SetOuResponseWordRequestData request{
                set_response_word_board_id->text().toStdString(),
                set_response_word_index->value(),
                set_response_word_channel->currentData().toInt(),
                set_response_word_ou_address->value(),
                set_response_word->value()
        };

        try
        {
            mko_client->set_ou_response_word(request);
            append_log(
                    QString{"SetOuResponseWord: успешно board=%1 index=%2 channel=%3 ou=%4 response_word=%5"}
                            .arg(set_response_word_board_id->text())
                            .arg(set_response_word_index->value())
                            .arg(set_response_word_channel->currentData().toInt())
                            .arg(set_response_word_ou_address->value())
                            .arg(set_response_word->value())
            );
        }
        catch (const std::exception &exception)
        {
            append_log(QString{"SetOuResponseWord: ошибка: %1"}.arg(exception.what()));
        }
    });

    QGroupBox *clear_receive_group{create_group_box("ОУ: ClearReceiveBuffer")};
    QGridLayout *clear_receive_layout{new QGridLayout{clear_receive_group}};

    QLineEdit *clear_receive_board_id{create_line_edit("board-1")};
    QSpinBox *clear_receive_index{create_spin_box(1, 2, 2)};

    add_labeled_widget(clear_receive_layout, 0, "Board ID", clear_receive_board_id);
    add_labeled_widget(clear_receive_layout, 1, "МКО", clear_receive_index);

    QPushButton *clear_receive_button{new QPushButton{"Очистить буфер приема", clear_receive_group}};
    clear_receive_layout->addWidget(clear_receive_button, 2, 0, 1, 2);

    QObject::connect(clear_receive_button,
                     &QPushButton::clicked,
                     [this, clear_receive_board_id, clear_receive_index]() {
        if (mko_client == nullptr)
        {
            append_log("ClearReceiveBuffer: gRPC-клиент МКО не создан.");
            return;
        }

        ClearBufferRequestData request{
                clear_receive_board_id->text().toStdString(),
                clear_receive_index->value()
        };

        try
        {
            mko_client->clear_receive_buffer(request);
            append_log(
                    QString{"ClearReceiveBuffer: успешно board=%1 index=%2"}
                            .arg(clear_receive_board_id->text())
                            .arg(clear_receive_index->value())
            );
        }
        catch (const std::exception &exception)
        {
            append_log(QString{"ClearReceiveBuffer: ошибка: %1"}.arg(exception.what()));
        }
    });

    QGroupBox *clear_transmit_group{create_group_box("ОУ: ClearTransmitBuffer")};
    QGridLayout *clear_transmit_layout{new QGridLayout{clear_transmit_group}};

    QLineEdit *clear_transmit_board_id{create_line_edit("board-1")};
    QSpinBox *clear_transmit_index{create_spin_box(1, 2, 2)};

    add_labeled_widget(clear_transmit_layout, 0, "Board ID", clear_transmit_board_id);
    add_labeled_widget(clear_transmit_layout, 1, "МКО", clear_transmit_index);

    QPushButton *clear_transmit_button{new QPushButton{"Очистить буфер передачи", clear_transmit_group}};
    clear_transmit_layout->addWidget(clear_transmit_button, 2, 0, 1, 2);

    QObject::connect(clear_transmit_button,
                     &QPushButton::clicked,
                     [this, clear_transmit_board_id, clear_transmit_index]() {
        if (mko_client == nullptr)
        {
            append_log("ClearTransmitBuffer: gRPC-клиент МКО не создан.");
            return;
        }

        ClearBufferRequestData request{
                clear_transmit_board_id->text().toStdString(),
                clear_transmit_index->value()
        };

        try
        {
            mko_client->clear_transmit_buffer(request);
            append_log(
                    QString{"ClearTransmitBuffer: успешно board=%1 index=%2"}
                            .arg(clear_transmit_board_id->text())
                            .arg(clear_transmit_index->value())
            );
        }
        catch (const std::exception &exception)
        {
            append_log(QString{"ClearTransmitBuffer: ошибка: %1"}.arg(exception.what()));
        }
    });

    panel_layout->addWidget(configure_group);
    panel_layout->addWidget(subscribe_group);
    panel_layout->addWidget(read_group);
    panel_layout->addWidget(write_group);
    panel_layout->addWidget(send_raw_group);
    panel_layout->addWidget(set_response_word_group);
    panel_layout->addWidget(clear_receive_group);
    panel_layout->addWidget(clear_transmit_group);
    panel_layout->addStretch();

    scroll_area->setWidget(panel);
    return scroll_area;
}

QWidget *QtViewMKO::create_menu_panel()
{
    QGroupBox *group{create_group_box("Меню")};
    QHBoxLayout *layout{new QHBoxLayout{group}};

    QPushButton *reset_button{new QPushButton{"Сбросить параметры", group}};
    layout->addWidget(reset_button);
    layout->addStretch();

    QObject::connect(reset_button, &QPushButton::clicked, [this]() {
        reset_parameters_to_defaults();
    });

    return group;
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
        line_edit->setProperty("default_text", placeholder);
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
        spin_box->setProperty("default_value", value);
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
        combo_box->setProperty("default_index", 0);
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
        table->setProperty("reset_words_table", true);
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

QString QtViewMKO::format_word(uint32_t word) const
{
    return QString{"0x%1"}.arg(word, 4, 16, QChar{'0'}).toUpper();
}

QString QtViewMKO::format_words(const std::vector<uint32_t> &words) const
{
    QStringList formatted_words;
    for (size_t index{0}; index < words.size(); ++index)
    {
        formatted_words << QString{"СД%1=%2"}.arg(index + 1).arg(format_word(words[index]));
    }
    return formatted_words.join(", ");
}

void QtViewMKO::append_ou_subaddress_data(const OuSubaddressData &data)
{
    append_log(
            QString{"OuSubaddressData: subaddress=%1 cmd_word=%2 result_word=%3 decoded_result=\"%4\""}
                    .arg(data.subaddress)
                    .arg(format_word(data.cmd_word))
                    .arg(format_word(data.result_word))
                    .arg(QString::fromStdString(data.decoded_result))
    );
    append_log(QString{"OuSubaddressData.sd: [%1]"}.arg(format_words(data.sd)));
}

void QtViewMKO::append_ou_command_event(const OuCommandEventData &data)
{
    append_log(
            QString{"OuCommandEvent: cmd_word=%1 result_word=%2 receive_from_ou=%3 ou_address=%4 "
                    "subaddress=%5 word_count=%6 decoded_command=\"%7\" decoded_result=\"%8\""}
                    .arg(format_word(data.cmd_word))
                    .arg(format_word(data.result_word))
                    .arg(data.receive_from_ou ? "true" : "false")
                    .arg(data.ou_address)
                    .arg(data.subaddress)
                    .arg(data.word_count)
                    .arg(QString::fromStdString(data.decoded_command))
                    .arg(QString::fromStdString(data.decoded_result))
    );
}

void QtViewMKO::sync_mko_indices(QSpinBox *const changed_spin_box)
{
    if (is_updating_mko_indices || kk_index_spin_box == nullptr || ou_index_spin_box == nullptr)
    {
        return;
    }

    QSpinBox *other_spin_box{changed_spin_box == kk_index_spin_box ? ou_index_spin_box : kk_index_spin_box};
    if (changed_spin_box == nullptr || changed_spin_box->value() != other_spin_box->value())
    {
        return;
    }

    is_updating_mko_indices = true;
    other_spin_box->setValue(alternate_mko_index(changed_spin_box->value()));
    is_updating_mko_indices = false;

    append_log(
            QString{"МКО %1 уже выбран для режима %2. Для режима %3 установлен МКО %4."}
                    .arg(changed_spin_box->value())
                    .arg(changed_spin_box == kk_index_spin_box ? "КК" : "ОУ")
                    .arg(other_spin_box == kk_index_spin_box ? "КК" : "ОУ")
                    .arg(other_spin_box->value())
    );
}

int QtViewMKO::alternate_mko_index(int index)
{
    return index == 1 ? 2 : 1;
}

void QtViewMKO::reset_parameters_to_defaults()
{
    if (central_widget == nullptr)
    {
        return;
    }

    is_updating_mko_indices = true;

    const QList<QLineEdit *> line_edits{central_widget->findChildren<QLineEdit *>()};
    for (QLineEdit *const line_edit : line_edits)
    {
        if (line_edit->property("default_text").isValid())
        {
            line_edit->setText(line_edit->property("default_text").toString());
        }
    }

    const QList<QSpinBox *> spin_boxes{central_widget->findChildren<QSpinBox *>()};
    for (QSpinBox *const spin_box : spin_boxes)
    {
        if (spin_box->property("default_value").isValid())
        {
            spin_box->setValue(spin_box->property("default_value").toInt());
        }
    }

    const QList<QComboBox *> combo_boxes{central_widget->findChildren<QComboBox *>()};
    for (QComboBox *const combo_box : combo_boxes)
    {
        if (combo_box->property("default_index").isValid())
        {
            combo_box->setCurrentIndex(combo_box->property("default_index").toInt());
        }
    }

    const QList<QTableWidget *> tables{central_widget->findChildren<QTableWidget *>()};
    for (QTableWidget *const table : tables)
    {
        if (!table->property("reset_words_table").toBool())
        {
            continue;
        }

        for (int row{0}; row < table->rowCount(); ++row)
        {
            QTableWidgetItem *item{table->item(row, 1)};
            if (item == nullptr)
            {
                item = new QTableWidgetItem{};
                table->setItem(row, 1, item);
            }
            item->setText("0");
        }
    }

    is_updating_mko_indices = false;
    append_log("Параметры вкладки МКО сброшены к значениям по умолчанию.");
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

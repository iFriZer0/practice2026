#include <iostream>
#include <new>

#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QLabel>
#include <QString>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QSpinBox>

#include "qt_view_rs_485.h"

QtViewRS485::QtViewRS485(QStackedWidget *const stacked_widget) noexcept
    : stacked_widget{stacked_widget},
      subscribed{false}
{
    central_widget = create_widget();
    setup_ui();
}

void QtViewRS485::show()
{
    if (stacked_widget->indexOf(central_widget) == NOT_FOUND)
    {
        stacked_widget->addWidget(central_widget);
    }

    stacked_widget->setCurrentWidget(central_widget);
}

void QtViewRS485::setup_ui()
{
    QVBoxLayout *main_layout = create_v_box_layout(central_widget);

    main_layout->addWidget(create_label("RS-485"));

    QGroupBox *connection_group = new QGroupBox("Подключение к RS-485 драйверу", central_widget);
    QHBoxLayout *connection_layout = new QHBoxLayout(connection_group);

    connection_layout->addWidget(create_label("gRPC endpoint:"));

    driver_endpoint_input = new QLineEdit(connection_group);
    driver_endpoint_input->setText("127.0.0.1:50051");
    connection_layout->addWidget(driver_endpoint_input);

    connection_group->setLayout(connection_layout);
    main_layout->addWidget(connection_group);

    QGroupBox *send_group = new QGroupBox("Отправка данных SendData", central_widget);
    QVBoxLayout *send_layout = new QVBoxLayout(send_group);

    QHBoxLayout *send_settings_layout = new QHBoxLayout();

    send_settings_layout->addWidget(create_label("Канал UART:"));

    channel_input = new QSpinBox(send_group);
    channel_input->setRange(0, 15);
    channel_input->setValue(0);
    send_settings_layout->addWidget(channel_input);

    send_layout->addLayout(send_settings_layout);

    QHBoxLayout *data_layout = new QHBoxLayout();

    data_layout->addWidget(create_label("Байты:"));

    bytes_input = new QLineEdit(send_group);
    bytes_input->setPlaceholderText("Например: 01 02 AA FF");
    data_layout->addWidget(bytes_input);

    send_button = new QPushButton("Отправить", send_group);
    data_layout->addWidget(send_button);

    send_layout->addLayout(data_layout);

    send_group->setLayout(send_layout);
    main_layout->addWidget(send_group);

    QGroupBox *subscribe_group = new QGroupBox("Прием данных Subscribe", central_widget);
    QHBoxLayout *subscribe_layout = new QHBoxLayout(subscribe_group);

    subscribe_button = new QPushButton("Запустить подписку", subscribe_group);
    subscribe_layout->addWidget(subscribe_button);

    subscribe_group->setLayout(subscribe_layout);
    main_layout->addWidget(subscribe_group);

    QGroupBox *sent_group = new QGroupBox("Отправленные данные", central_widget);
    QVBoxLayout *sent_layout = new QVBoxLayout(sent_group);

    sent_log = new QTextEdit(sent_group);
    sent_log->setReadOnly(true);
    sent_layout->addWidget(sent_log);

    sent_group->setLayout(sent_layout);
    main_layout->addWidget(sent_group);

    QGroupBox *received_group = new QGroupBox("Принятые данные", central_widget);
    QVBoxLayout *received_layout = new QVBoxLayout(received_group);

    received_log = new QTextEdit(received_group);
    received_log->setReadOnly(true);
    received_layout->addWidget(received_log);

    received_group->setLayout(received_layout);
    main_layout->addWidget(received_group);

    status_label = create_label("Статус: готово");
    main_layout->addWidget(status_label);

    central_widget->setLayout(main_layout);

    QObject::connect(send_button, &QPushButton::clicked, [this]() {
        on_send_clicked();
    });

    QObject::connect(subscribe_button, &QPushButton::clicked, [this]() {
        on_subscribe_clicked();
    });
}

void QtViewRS485::on_send_clicked()
{
    const QString endpoint = driver_endpoint_input->text();
    const int channel = channel_input->value();
    const QString bytes = bytes_input->text();

    append_sent_log(
        "endpoint: " + endpoint +
        "\nchannel_id: " + QString::number(channel) +
        "\ndata: " + bytes +
        "\nstatus: SendData пока не подключен к gRPC\n"
    );

    status_label->setText("Статус: SendData пока не подключен к gRPC");

    /*
        Позже здесь будет вызов логического слоя:

        rsService->sendData(
            endpoint.toStdString(),
            channel,
            bytes.toStdString()
        );

        Интерфейс только берет данные у пользователя.
        Проверка корректности, парсинг байтов и обработка ошибок —
        в service/driver слое.
    */
}

void QtViewRS485::on_subscribe_clicked()
{
    subscribed = !subscribed;

    if (subscribed)
    {
        subscribe_button->setText("Остановить подписку");

        append_received_log("Subscribe запущен. Ожидание данных от RS-485 драйвера...\n");

        status_label->setText("Статус: Subscribe пока не подключен к gRPC");

        /*
            Позже здесь будет запуск чтения gRPC stream.

            Важно:
            чтение Subscribe нельзя делать в UI-потоке,
            иначе окно зависнет.
            Нужно будет вынести в отдельный поток или асинхронный клиент.
        */
    }
    else
    {
        subscribe_button->setText("Запустить подписку");

        append_received_log("Subscribe остановлен.\n");

        status_label->setText("Статус: готово");

        /*
            Позже здесь будет остановка подписки:
            cancel context / остановить поток чтения.
        */
    }
}

void QtViewRS485::append_sent_log(const QString &text)
{
    sent_log->append(text);
}

void QtViewRS485::append_received_log(const QString &text)
{
    received_log->append(text);
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
#include "qt_view_rs_485.h"

#include <exception>
#include <iostream>
#include <new>
#include <string>
#include <vector>

#include <QFileDialog>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QMetaObject>

QtViewRS485::QtViewRS485(
    QStackedWidget *const stacked_widget
)
    : stacked_widget{stacked_widget},
      rs485_client_{
          std::make_shared<
              Rs485MicroserviceClient
          >()
      }
{
    central_widget = create_widget();
    setup_ui();
    connect_to_microservice();
}

QtViewRS485::~QtViewRS485()
{
    if (rs485_client_)
    {
        rs485_client_->disconnect();
    }
}

void QtViewRS485::show()
{
    if (stacked_widget->indexOf(
            central_widget
        ) == NOT_FOUND)
    {
        stacked_widget->addWidget(
            central_widget
        );
    }

    stacked_widget->setCurrentWidget(
        central_widget
    );
}

void QtViewRS485::setup_ui()
{
    QVBoxLayout *main_layout =
        create_v_box_layout(central_widget);

    main_layout->addWidget(
        create_label("RS-485")
    );

    QGroupBox *send_group =
        new QGroupBox(
            "Отправка данных SendData",
            central_widget
        );

    QVBoxLayout *send_layout =
        new QVBoxLayout(send_group);

    QHBoxLayout *send_settings_layout =
        new QHBoxLayout();

    send_settings_layout->addWidget(
        create_label("Канал UART:")
    );

    channel_input =
        new QSpinBox(send_group);

    channel_input->setRange(0, 15);
    channel_input->setValue(0);

    send_settings_layout->addWidget(
        channel_input
    );

    send_settings_layout->addStretch();

    send_layout->addLayout(
        send_settings_layout
    );

    QHBoxLayout *data_layout =
        new QHBoxLayout();

    data_layout->addWidget(
        create_label("Байты:")
    );

    bytes_input =
        new QLineEdit(send_group);

    bytes_input->setPlaceholderText(
        "Например: 01 02 AA FF"
    );

    data_layout->addWidget(
        bytes_input
    );

    send_layout->addLayout(
        data_layout
    );

    QHBoxLayout *file_layout =
        new QHBoxLayout();

    file_layout->addWidget(
        create_label("Файл:")
    );

    file_path_input =
        new QLineEdit(send_group);

    file_path_input->setReadOnly(true);
    file_path_input->setPlaceholderText(
        "Файл не выбран"
    );

    file_layout->addWidget(
        file_path_input
    );

    browse_button =
        new QPushButton(
            "Обзор...",
            send_group
        );

    file_layout->addWidget(
        browse_button
    );

    send_layout->addLayout(
        file_layout
    );

    send_button =
        new QPushButton(
            "Отправить",
            send_group
        );

    send_layout->addWidget(
        send_button
    );

    main_layout->addWidget(
        send_group
    );

    QGroupBox *subscribe_group =
        new QGroupBox(
            "Прием данных Subscribe",
            central_widget
        );

    QHBoxLayout *subscribe_layout =
        new QHBoxLayout(subscribe_group);

    subscribe_button =
        new QPushButton(
            "Запустить подписку",
            subscribe_group
        );

    subscribe_layout->addWidget(
        subscribe_button
    );

    main_layout->addWidget(
        subscribe_group
    );

    QHBoxLayout *logs_layout =
        new QHBoxLayout();

    QGroupBox *sent_group =
        new QGroupBox(
            "Отправленные данные",
            central_widget
        );

    QVBoxLayout *sent_layout =
        new QVBoxLayout(sent_group);

    sent_log =
        new QTextEdit(sent_group);

    sent_log->setReadOnly(true);
    sent_log->setMinimumHeight(180);

    sent_layout->addWidget(
        sent_log
    );

    logs_layout->addWidget(
        sent_group
    );

    QGroupBox *received_group =
        new QGroupBox(
            "Принятые данные",
            central_widget
        );

    QVBoxLayout *received_layout =
        new QVBoxLayout(received_group);

    received_log =
        new QTextEdit(received_group);

    received_log->setReadOnly(true);
    received_log->setMinimumHeight(180);

    received_layout->addWidget(
        received_log
    );

    logs_layout->addWidget(
        received_group
    );

    main_layout->addLayout(
        logs_layout
    );

    status_label =
        create_label(
            "Статус: микросервис не подключен"
        );

    main_layout->addWidget(
        status_label
    );

    QObject::connect(
        browse_button,
        &QPushButton::clicked,
        [this]()
        {
            on_browse_clicked();
        }
    );

    QObject::connect(
        send_button,
        &QPushButton::clicked,
        [this]()
        {
            on_send_clicked();
        }
    );

    QObject::connect(
        subscribe_button,
        &QPushButton::clicked,
        [this]()
        {
            on_subscribe_clicked();
        }
    );

    QObject::connect(
        bytes_input,
        &QLineEdit::textEdited,
        [this](const QString &)
        {
            file_path_input->clear();
        }
    );

    set_driver_controls_enabled(false);
}

void QtViewRS485::set_driver_controls_enabled(
    bool enabled
)
{
    channel_input->setEnabled(enabled);
    bytes_input->setEnabled(enabled);
    file_path_input->setEnabled(enabled);
    browse_button->setEnabled(enabled);
    send_button->setEnabled(enabled);
    subscribe_button->setEnabled(enabled);
}

void QtViewRS485::connect_to_microservice()
{
    const QString endpoint =
        QString::fromLatin1(
            DEFAULT_RS485_SERVICE_ADDRESS
        );

    try
    {
        const bool connected =
            rs485_client_->connect(
                DEFAULT_RS485_SERVICE_ADDRESS
            );

        if (connected)
        {
            set_driver_controls_enabled(true);

            status_label->setText(
                "Статус: подключено к микросервису "
                + endpoint
            );
        }
        else
        {
            set_driver_controls_enabled(false);

            status_label->setText(
                "Статус: не удалось подключиться "
                "к микросервису "
                + endpoint
            );
        }
    }
    catch (const std::exception &error)
    {
        set_driver_controls_enabled(false);

        status_label->setText(
            "Статус: "
            + QString::fromStdString(
                error.what()
            )
        );
    }
}

void QtViewRS485::on_browse_clicked()
{
    const QString file_name =
        QFileDialog::getOpenFileName(
            central_widget,
            "Выберите файл с байтами",
            QString(),
            "All files (*)"
        );

    if (file_name.isEmpty())
    {
        return;
    }

    file_path_input->setText(
        file_name
    );

    bytes_input->clear();

    status_label->setText(
        "Статус: файл выбран"
    );
}

void QtViewRS485::on_send_clicked()
{
    const uint32_t channel_id =
        static_cast<uint32_t>(
            channel_input->value()
        );

    const QString file_path =
        file_path_input->text().trimmed();

    const QString bytes_text =
        bytes_input->text().trimmed();

    QString source_text;
    QString sent_data_text;

    try
    {
        Rs485SendResult result;

        if (!file_path.isEmpty())
        {
            result =
                rs485_client_->sendDataFromFile(
                    channel_id,
                    file_path.toStdString()
                );

            source_text =
                "file: " + file_path;

            sent_data_text =
                "binary file";
        }
        else
        {
            result =
                rs485_client_->sendData(
                    channel_id,
                    bytes_text.toStdString()
                );

            source_text =
                "manual input";

            sent_data_text =
                bytes_text;
        }

        QString log_entry;

        log_entry += "channel_id: ";
        log_entry += QString::number(
            result.channel_id
        );

        log_entry += "\nsource: ";
        log_entry += source_text;

        log_entry += "\ndata: ";
        log_entry += sent_data_text;

        log_entry += "\nsuccess: ";
        log_entry +=
            result.success ? "true" : "false";

        log_entry += "\nmessage: ";
        log_entry += QString::fromStdString(
            result.error_message
        );

        log_entry += "\n";

        append_sent_log(log_entry);

        if (result.success)
        {
            status_label->setText(
                "Статус: данные отправлены"
            );
        }
        else
        {
            status_label->setText(
                "Статус: ошибка отправки — "
                + QString::fromStdString(
                    result.error_message
                )
            );
        }
    }
    catch (const std::exception &error)
    {
        append_sent_log(
            "channel_id: "
            + QString::number(channel_id)
            + "\nsource: "
            + (
                file_path.isEmpty()
                    ? QString{"manual input"}
                    : QString{"file: "} + file_path
              )
            + "\ndata: "
            + sent_data_text
            + "\nexception: "
            + QString::fromStdString(
                error.what()
            )
            + "\n"
        );

        status_label->setText(
            "Статус: "
            + QString::fromStdString(
                error.what()
            )
        );
    }
}

void QtViewRS485::on_subscribe_clicked()
{
    if (!subscribed)
    {
        try
        {
            rs485_client_->startSubscribe(
                [this](
                    const Rs485ReceiveResult &result
                )
                {
                    QMetaObject::invokeMethod(
                        central_widget,
                        [this, result]()
                        {
                            handle_received_data(
                                result
                            );
                        },
                        Qt::QueuedConnection
                    );
                }
            );

            subscribed = true;

            subscribe_button->setText(
                "Остановить подписку"
            );

            append_received_log(
                "Subscribe started. "
                "Waiting for data...\n"
            );

            status_label->setText(
                "Статус: подписка запущена"
            );
        }
        catch (const std::exception &error)
        {
            subscribed = false;

            status_label->setText(
                "Статус: "
                + QString::fromStdString(
                    error.what()
                )
            );
        }
    }
    else
    {
        try
        {
            rs485_client_->stopSubscribe();

            subscribed = false;

            subscribe_button->setText(
                "Запустить подписку"
            );

            append_received_log(
                "Subscribe stopped.\n"
            );

            status_label->setText(
                "Статус: подписка остановлена"
            );
        }
        catch (const std::exception &error)
        {
            status_label->setText(
                "Статус: "
                + QString::fromStdString(
                    error.what()
                )
            );
        }
    }
}

void QtViewRS485::handle_received_data(
    const Rs485ReceiveResult &result
)
{
    QString log_entry;

    log_entry += "channel_id: ";
    log_entry += QString::number(
        result.channel_id
    );

    log_entry += "\nsuccess: ";
    log_entry +=
        result.success ? "true" : "false";

    log_entry += "\ndata: ";
    log_entry += bytes_to_hex(
        result.data
    );

    log_entry += "\nmessage: ";
    log_entry += QString::fromStdString(
        result.error_message
    );

    log_entry += "\n";

    append_received_log(log_entry);

    if (result.success)
    {
        status_label->setText(
            "Статус: получены данные"
        );
    }
    else
    {
        status_label->setText(
            "Статус: ошибка приема — "
            + QString::fromStdString(
                result.error_message
            )
        );
    }
}

QString QtViewRS485::bytes_to_hex(
    const std::vector<uint8_t> &bytes
)
{
    QString result;

    for (std::size_t index = 0;
         index < bytes.size();
         ++index)
    {
        result += QStringLiteral("%1").arg(
            static_cast<unsigned int>(
                bytes[index]
            ),
            2,
            16,
            QLatin1Char('0')
        ).toUpper();

        if (index + 1 < bytes.size())
        {
            result += " ";
        }
    }

    return result;
}

void QtViewRS485::append_sent_log(
    const QString &text
)
{
    sent_log->append(text);
}

void QtViewRS485::append_received_log(
    const QString &text
)
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
        std::cerr
            << "Failed to create QWidget."
            << std::endl;
    }

    return widget;
}

QVBoxLayout *QtViewRS485::create_v_box_layout(
    QWidget *const parent
) const
{
    QVBoxLayout *layout{nullptr};

    try
    {
        layout = new QVBoxLayout{parent};
    }
    catch (const std::bad_alloc &)
    {
        std::cerr
            << "Failed to create QVBoxLayout."
            << std::endl;
    }

    return layout;
}

QLabel *QtViewRS485::create_label(
    const QString &text
) const
{
    QLabel *label{nullptr};

    try
    {
        label = new QLabel{text};
    }
    catch (const std::bad_alloc &)
    {
        std::cerr
            << "Failed to create QLabel."
            << std::endl;
    }

    return label;
}

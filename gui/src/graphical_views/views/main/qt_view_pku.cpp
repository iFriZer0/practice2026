#include <QComboBox>
#include <QTextEdit>
#include <algorithm>
#include <cctype>
#include <chrono>
#include <grpc/grpc.h>
#include <grpcpp/client_context.h>
#include <grpcpp/support/status.h>
#include <iostream>
#include <fstream>
#include <memory>
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
#include <QGroupBox>
#include <QGridLayout>
#include <QRegularExpressionValidator>
#include <QRegularExpression>
#include <QLatin1Char>
#include <QByteArray>
#include <QListWidget>
#include <QAbstractItemView>
#include <QList>
#include <QListWidgetItem>
#include <QStringList>
#include <QObject>
#include <climits>
#include <grpcpp/grpcpp.h>
#include <string>
#include "error.h"
#include "pku_service.pb.h"
#include "pku_service.grpc.pb.h"


#include "qt_view_pku.h"

using namespace api;

ConnectionError::ConnectionError(const char *message, const std::type_info &first_error) noexcept
    : Error(message, first_error) {}

const void *ConnectionError::get_data() const noexcept
{
    return nullptr;
}

QGroupBox *QtViewPKU::create_group_box(const QString &title, QWidget *const parent) const noexcept
{
    QGroupBox *group_box{nullptr};
    try {
        group_box = new QGroupBox{title, parent};
    }
    catch (const std::bad_alloc &) {
        std::cerr << "Не удалось создать QGroupBox" << std::endl;
    }
    return group_box;
}

QGridLayout *QtViewPKU::create_grid_layout(QWidget *const parent) const noexcept
{
    QGridLayout *grid_layout{nullptr};
    try {
        grid_layout = new QGridLayout{parent};
    }
    catch (const std::bad_alloc &) {
        std::cerr << "Не удалось создать QGridLayout" << std::endl;
    }
    return grid_layout;
}

QCheckBox *QtViewPKU::create_check_box(const QString &text, QWidget *const parent) const noexcept
{
    QCheckBox *check_box{nullptr};
    try {
        check_box = new QCheckBox{text, parent};
    }
    catch (const std::bad_alloc &) {
        std::cerr << "Не удалось создать QCheckBox" << std::endl;
    }
    return check_box;
}

QRegularExpressionValidator *QtViewPKU::create_regular_expression_validator(const QRegularExpression &re, QObject *const parent) const noexcept
{
    QRegularExpressionValidator *regular_expression_validator{nullptr};
    try {
        regular_expression_validator = new QRegularExpressionValidator{re, parent};
    }
    catch (const std::bad_alloc &) {
        std::cerr << "Не удалось создать QRegularExpressionValidator" << std::endl;
    }
    return regular_expression_validator;
}

QScrollArea *QtViewPKU::create_scroll_area(QWidget *const parent) const noexcept
{
    QScrollArea *scroll_area{nullptr};
    try {
        scroll_area = new QScrollArea{parent};
    }
    catch (const std::bad_alloc &) {
        std::cerr << "Не удалось создать QScrollArea" << std::endl;
    }
    return scroll_area;
}

QListWidget *QtViewPKU::create_list_widget(QWidget *const parent) const noexcept
{
    QListWidget *list_widget{nullptr};
    try {
        list_widget = new QListWidget{parent};
    }
    catch (const std::bad_alloc &) {
        std::cerr << "Не удалось создать QListWidget" << std::endl;
    }
    return list_widget;
}

QIntValidator *QtViewPKU::create_int_validator(const int minimum, const int maximum, QObject *const parent) const noexcept
{
    QIntValidator *int_validator{nullptr};
    try {
        int_validator = new QIntValidator{minimum, maximum, parent};
    }
    catch (const std::bad_alloc &) {
        std::cerr << "Не удалось QIntValidator" << std::endl;
    }
    return int_validator;
}

QString QtViewPKU::parse_mac(const QString &mac) const noexcept
{
    QString result;
    try {
        QByteArray bytes{mac.toLatin1()};
        result = QString{"%1:%2:%3:%4:%5:%6"}
                .arg(static_cast<unsigned char>(bytes[0]), 2, 16, QLatin1Char{'0'})
                .arg(static_cast<unsigned char>(bytes[1]), 2, 16, QLatin1Char{'0'})
                .arg(static_cast<unsigned char>(bytes[2]), 2, 16, QLatin1Char{'0'})
                .arg(static_cast<unsigned char>(bytes[3]), 2, 16, QLatin1Char{'0'})
                .arg(static_cast<unsigned char>(bytes[4]), 2, 16, QLatin1Char{'0'})
                .arg(static_cast<unsigned char>(bytes[5]), 2, 16, QLatin1Char{'0'})
                .toUpper();
    }
    catch (const std::bad_alloc &) {
        std::cerr << "Не удалось преобразовать MAC-адрес." << std::endl;
    }
    return result;
}

QString QtViewPKU::parse_address(const QString &address) const noexcept
{
    QString result;
    try {
        QByteArray bytes{address.toLatin1()};
        result = QString{"%1.%2.%3.%4"}
                .arg(static_cast<unsigned char>(bytes[0]))
                .arg(static_cast<unsigned char>(bytes[1]))
                .arg(static_cast<unsigned char>(bytes[2]))
                .arg(static_cast<unsigned char>(bytes[3]));
    }
    catch (const std::bad_alloc &) {
        std::cerr << "Не удалось преобразовать IP-адрес." << std::endl;
    }
    return result;
}

std::string read_pku_service_address() {
    std::ifstream file("../../pku/src/configuration/pku_service_address.txt");
    std::string ip;
    if (file.is_open() && std::getline(file, ip)) {
        ip.erase(std::remove_if(ip.begin(), ip.end(), ::isspace), ip.end());
    }
    std::string port;
    if (std::getline(file, port)) {
        port.erase(std::remove_if(port.begin(), port.end(), isspace), port.end());
    }
    return ip + ":" + port;
}

QtViewPKU::QtViewPKU(QStackedWidget *const stacked_widget)
    : central_widget{create_widget()}, stacked_widget{stacked_widget}
{
    QScrollArea *scroll_area{create_scroll_area()};
    scroll_area->setWidgetResizable(true);
    scroll_area->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    std::string target_address = read_pku_service_address();
    channel_ = grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials());

    api::CommandRequest request;
    request.set_command_id(1);
    api::CommandResponse response;
    grpc::ClientContext context;
    std::unique_ptr<MainService::Stub> stub{api::MainService::NewStub(channel_)};
    std::chrono::system_clock::time_point deadline{std::chrono::system_clock::now() + std::chrono::seconds(2)};
    context.set_deadline(deadline);
    grpc::Status status{stub->SendCommand(&context, request, &response)};
    if (!status.ok()) {
        throw ConnectionError{"Connection was not established", typeid(ConnectionError)};
    }

    QWidget *content_widget{create_widget()};
    QVBoxLayout *main_layout{create_v_box_layout(content_widget)};

    main_layout->addWidget(create_label("Пульт контроля и управления (РК/ПКУ)"));
    main_layout->addWidget(create_label("<b>Статус и связь</b>"));

    QHBoxLayout *status_buttons_layout = create_h_box_layout();
    QPushButton *btn_check_conn = create_button("Проверить соединение", central_widget);
    btn_check_conn->setFixedWidth(200);
    status_buttons_layout->addWidget(btn_check_conn);

    QPushButton *btn_get_status = create_button("Статус оборудования", central_widget);
    btn_get_status->setFixedWidth(200);
    status_buttons_layout->addWidget(btn_get_status);

    QPushButton *btn_get_version = create_button("Проверить версию", central_widget);
    btn_get_version->setFixedWidth(200);
    status_buttons_layout->addWidget(btn_get_version);

    status_buttons_layout->addStretch();
    main_layout->addLayout(status_buttons_layout);

    QHBoxLayout *status_labels_layout = create_h_box_layout();
    QLabel *lbl_connect_status = create_label("Статус: не проверено");
    lbl_connect_status->setFixedWidth(200);
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
    main_layout->addWidget(create_label("<b>Основная информация</b>"));

    QHBoxLayout *main_info_buttons_layout = create_h_box_layout();
    QPushButton *btn_read_main = create_button("Читать основную информацию", central_widget);
    btn_read_main->setFixedWidth(250);
    main_info_buttons_layout->addWidget(btn_read_main);

    QPushButton *btn_write_main = create_button("Записать основную информацию", central_widget);
    btn_write_main->setFixedWidth(250);
    main_info_buttons_layout->addWidget(btn_write_main);

    QGroupBox *info_group{create_group_box("Параметры основной информации", central_widget)};
    QGridLayout *grid_layout{create_grid_layout(info_group)};

    grid_layout->addWidget(create_label("Описание (до 120 символов)"), 0, 0);
    QLineEdit *le_description{create_line_edit(central_widget)};
    le_description->setMaxLength(120);
    le_description->setMinimumWidth(300);
    grid_layout->addWidget(le_description, 0, 1, 1, 2);

    grid_layout->addWidget(create_label("MAC-адрес"), 1, 0);
    QLineEdit *le_mac{create_line_edit(central_widget)};
    le_mac->setPlaceholderText("AA:BB:CC:DD:EE:FF");
    le_mac->setValidator(create_regular_expression_validator(QRegularExpression{"^([0-9A-Fa-f]{2}:){5}[0-9A-Fa-f]{2}$"}, stacked_widget));
    grid_layout->addWidget(le_mac, 1, 1);

    QRegularExpression ip_regex{"^((25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)$"};

    grid_layout->addWidget(create_label("IP-адрес"), 2, 0);
    QLineEdit *le_ip{create_line_edit(central_widget)};
    le_ip->setPlaceholderText("192.168.1.1");
    le_ip->setValidator(create_regular_expression_validator(ip_regex, stacked_widget));
    grid_layout->addWidget(le_ip, 2, 1);

    grid_layout->addWidget(create_label("Сетевая маска"), 2, 2);
    QLineEdit *le_mask{create_line_edit(central_widget)};
    le_mask->setPlaceholderText("255.255.255.0");
    le_mask->setValidator(create_regular_expression_validator(ip_regex, stacked_widget));
    grid_layout->addWidget(le_mask, 2, 3);

    grid_layout->addWidget(create_label("Основной шлюз"), 3, 0);
    QLineEdit *le_gateway{create_line_edit(central_widget)};
    le_gateway->setPlaceholderText("192.168.1.1");
    le_gateway->setValidator(create_regular_expression_validator(ip_regex, stacked_widget));
    grid_layout->addWidget(le_gateway, 3, 1);

    grid_layout->addWidget(create_label("DNS"), 3, 2);
    QLineEdit *le_dns{create_line_edit(central_widget)};
    le_dns->setPlaceholderText("8.8.8.8");
    le_dns->setValidator(create_regular_expression_validator(ip_regex, stacked_widget));
    grid_layout->addWidget(le_dns, 3, 3);

    QCheckBox *cb_dhcp{create_check_box("Использовать DHCP", central_widget)};
    grid_layout->addWidget(cb_dhcp, 4, 0, 1, 2);

    main_layout->addWidget(info_group);

    main_info_buttons_layout->addStretch();
    main_layout->addLayout(main_info_buttons_layout);

    QHBoxLayout *main_info_labels_layout = create_h_box_layout();
    QLabel *lbl_read_main_status = create_label("Данные не прочитаны");
    lbl_read_main_status->setFixedWidth(250);
    main_info_labels_layout->addWidget(lbl_read_main_status);

    QLabel *lbl_write_main_status = create_label("Изменения не записаны");
    lbl_write_main_status->setFixedWidth(250);
    main_info_labels_layout->addWidget(lbl_write_main_status);

    main_info_labels_layout->addStretch();
    main_layout->addLayout(main_info_labels_layout);

    main_layout->addSpacing(15);

    QLabel *rk_num_label = create_label("Номер РК");
    QLineEdit *rk_num_input = create_line_edit(central_widget);
    rk_num_input->setValidator(create_regular_expression_validator(QRegularExpression("^([1-9]|[1-3][0-9]|4[0-8])$"), stacked_widget));
    rk_num_input->setPlaceholderText("1, 2, ..., 48");
    rk_num_input->setMinimumWidth(100);

    QLabel *rk_time_label = create_label("Длительность (мс)");
    QLineEdit *rk_time_input = create_line_edit(central_widget);
    rk_time_input->setValidator(create_int_validator(1, INT_MAX, stacked_widget));

    QPushButton *btn_send_rk = create_button("Выдать РК", central_widget);

    QLabel *chan_label = create_label("Выбор ПКУ");
    QComboBox *chan_select = create_combo_box(central_widget);
    for (int i = 1; i <= 64; ++i) { chan_select->addItem(QString("ПКУ %1").arg(i)); }

    QLabel *mode_label = create_label("Режим");
    QComboBox *mode_select = create_combo_box(central_widget);
    mode_select->addItem("0 - Отрицательный импульс");
    mode_select->addItem("1 - Положительный импульс");
    mode_select->addItem("2 - Фронт");

    QPushButton *btn_set_mode = create_button("Применить режим", central_widget);

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
    main_layout->addWidget(create_label("<b>Чтение длительностей ПКУ</b>"));

    QHBoxLayout *read_pku_layout = create_h_box_layout();
    QLabel *lbl_multi_pku = create_label("Выбор номеров ПКУ");
    read_pku_layout->addWidget(lbl_multi_pku);

    QListWidget *pku_list_widget{create_list_widget(central_widget)};
    pku_list_widget->setSelectionMode(QAbstractItemView::MultiSelection);
    for (int i{1}; i < 64; ++i) {
        pku_list_widget->addItem(QString::number(i));
    }
    pku_list_widget->setFixedWidth(250);
    read_pku_layout->addWidget(pku_list_widget);

    QPushButton *btn_read_pku = create_button("Читать длительности ПКУ", central_widget);
    btn_read_pku->setFixedWidth(220);
    read_pku_layout->addWidget(btn_read_pku);

    read_pku_layout->addStretch();
    main_layout->addLayout(read_pku_layout);

    QLabel *lbl_read_pku_status = create_label("Длительности ПКУ не считывались");
    main_layout->addWidget(lbl_read_pku_status);

    QTextEdit *pku_log = create_text_edit(central_widget);
    pku_log->setReadOnly(true);
    pku_log->setMinimumHeight(180);
    main_layout->addWidget(pku_log);


    QObject::connect(btn_check_conn, &QPushButton::clicked, [this, lbl_connect_status]() {
        lbl_connect_status->setText("Запрос...");

        ::api::CommandRequest request;
        request.set_command_id(1);

        ::api::CommandResponse response;
        grpc::ClientContext context;

        auto stub = ::api::MainService::NewStub(channel_);



        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            std::string result{response.success() ? "Соединение установлено" : "Ошибка \"" + response.result_text() + "\""};
            lbl_connect_status->setText(QString::fromStdString(result));
        } else {
            lbl_connect_status->setText("Ошибка сети");
        }
    });

    QObject::connect(btn_get_status, &QPushButton::clicked, [lbl_equipment_status, this]() {
        lbl_equipment_status->setText("Запрос...");

        api::CommandRequest request;
        request.set_command_id(2);

        api::CommandResponse response;
        grpc::ClientContext context;

        auto stub = ::api::MainService::NewStub(channel_);
        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            std::string result{response.success() ? "Соединение установлено" : "Ошибка \"" + response.result_text() + "\""};
            lbl_equipment_status->setText(QString::fromStdString(result));
        } else {
            lbl_equipment_status->setText("Ошибка сети");
        }
    });

    QObject::connect(btn_read_main, &QPushButton::clicked, [lbl_read_main_status, pku_log, this]() {
        lbl_read_main_status->setText("Чтение...");

        api::CommandRequest request;
        request.set_command_id(3);

        api::CommandResponse response;
        grpc::ClientContext context;

        auto stub = ::api::MainService::NewStub(channel_);
        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            if (!response.success()) {
                lbl_read_main_status->setText("Ошибка");
                pku_log->append(QString::fromStdString("Ошибка: \"" + response.result_text() + "\""));
            } else {
                QStringList parts = QString::fromStdString(response.result_text()).split(';');
                if (parts.size() >= 9) {
                    pku_log->append("Прочитана основная информация");
                    pku_log->append("Идентификатор модуля: " + parts[0]);
                    pku_log->append("Размер буфера на приём: " + parts[1]);
                    pku_log->append("Описание модуля: " + parts[2]);
                    pku_log->append("MAC-адрес: " + parse_mac(parts[3]));
                    pku_log->append("IP-адрес: " + parse_address(parts[4]));
                    pku_log->append("Маска подсети: " + parse_address(parts[5]));
                    pku_log->append("Основной шлюз: " + parse_address(parts[6]));
                    pku_log->append("DNS-сервер: " + parse_address(parts[7]));
                    pku_log->append("Флаг \"Использовать DHCP\": " + parts[8]);
                    lbl_read_main_status->setText("Успешно прочитано");
                } else {
                    lbl_read_main_status->setText("Не удалось прочитать всю основную информацию");
                }
            }
        } else {
            lbl_read_main_status->setText("Ошибка сети");
        }
    });

    QObject::connect(btn_write_main, &QPushButton::clicked, [lbl_write_main_status, le_description, le_mac, le_ip, le_mask, le_gateway, le_dns, cb_dhcp, this]() {
        QString description{le_description->text()};
        QString mac{le_mac->text()};
        QString ip{le_ip->text()};
        QString mask{le_mask->text()};
        QString gateway{le_gateway->text()};
        QString dns{le_dns->text()};
        QString dhcp{cb_dhcp->isChecked() ? "1" : "0"};

        if (description.isEmpty() || mac.isEmpty() || ip.isEmpty() || mask.isEmpty() || gateway.isEmpty() || dns.isEmpty()) {
            lbl_write_main_status->setText("Все поля должны быть заполнены");
        } else {
            lbl_write_main_status->setText("Запись...");

            api::CommandRequest request;
            request.set_command_id(4);
            request.set_command_param((description + ";" + mac + ";" + ip + ";" + mask + ";" + gateway + ";" + dns + ";" + dhcp + ";" + "op_id").toStdString());

            api::CommandResponse response;
            grpc::ClientContext context;

            auto stub = ::api::MainService::NewStub(channel_);
            grpc::Status status = stub->SendCommand(&context, request, &response);

            if (status.ok()) {
                std::string result{response.success() ? "Успешно записано" : "Ошибка \"" + response.result_text() + "\""};
                lbl_write_main_status->setText(QString::fromStdString(result));
            } else {
                lbl_write_main_status->setText("Ошибка записи");
            }
        }
    });

    QObject::connect(btn_read_pku, &QPushButton::clicked, [pku_list_widget, lbl_read_pku_status, pku_log, this]() {
        QList<QListWidgetItem *>selected{pku_list_widget->selectedItems()};
        if (selected.isEmpty()) {
            lbl_read_pku_status->setText("Не выбраны ПКУ");
        }
        QStringList indices;
        for (QListWidgetItem *item : selected) {
            indices << item->text();
        }
        QString param{indices.join(";")};
        lbl_read_pku_status->setText("Чтение...");

        api::CommandRequest request;
        request.set_command_id(5);
        request.set_command_param(param.toStdString());


        api::CommandResponse response;
        grpc::ClientContext context;

        auto stub = ::api::MainService::NewStub(channel_);

        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            if (!response.success()) {
                lbl_read_pku_status->setText("Ошибка чтения");
                pku_log->append(QString::fromStdString("Ошибка \"" + response.result_text() + "\""));
            } else {
                QStringList parts = QString::fromStdString(response.result_text()).split(';');
                lbl_read_pku_status->setText("Успешно прочитано");
                pku_log->append("--- Длительности ПКУ ---");
                for (int i{0}; i < parts.size() && i < indices.size(); ++i) {
                    pku_log->append(QString{"ПКУ %1: %2"}.arg(indices[i], parts[i]));
                }
            }
        } else {
            lbl_read_pku_status->setText("Ошибка сети");
        }
    });

    QObject::connect(btn_set_mode, &QPushButton::clicked, [chan_select, mode_select, lbl_set_mode_status, this]() {
        QString param = QString::number(chan_select->currentIndex() + 1) + ";" + QString::number(mode_select->currentIndex()) + ";" + "op_id";
        lbl_set_mode_status->setText("Применение...");

        api::CommandRequest request;
        request.set_command_id(6);
        request.set_command_param(param.toStdString());


        api::CommandResponse response;
        grpc::ClientContext context;

        auto stub = ::api::MainService::NewStub(channel_);

        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            std::string result{response.success() ? "Режим изменён" : "Ошибка \"" + response.result_text() + "\""};
            lbl_set_mode_status->setText(QString::fromStdString(result));
        } else {
            lbl_set_mode_status->setText("Ошибка сети");
        }
    });

    QObject::connect(btn_send_rk, &QPushButton::clicked, [rk_num_input, lbl_send_rk_status, rk_time_input, this]() {
        bool ok_num, ok_time;
        rk_num_input->text().toInt(&ok_num);
        if (!ok_num) {
            lbl_send_rk_status->setText("Заполните номер РК");
        } else {
            rk_time_input->text().toInt(&ok_time);
            if (!ok_time) {
                lbl_send_rk_status->setText("Заполните длительность");
            } else {
                QString param = rk_num_input->text() + ";" + rk_time_input->text() + ";" + "op_id";
                lbl_send_rk_status->setText("Отправка...");

                ::api::CommandRequest request;
                request.set_command_id(7);
                request.set_command_param(param.toStdString());

                ::api::CommandResponse response;
                grpc::ClientContext context;


                auto stub = ::api::MainService::NewStub(channel_);
                grpc::Status status = stub->SendCommand(&context, request, &response);

                if (status.ok()) {
                    std::string result{response.success() ? "РК отправлена" : "Ошибка \"" + response.result_text() + "\""};
                    lbl_send_rk_status->setText(QString::fromStdString(result));
                } else {
                    lbl_send_rk_status->setText("Ошибка сети");
                }
            }
        }
    });

    QObject::connect(btn_get_version, &QPushButton::clicked, [lbl_version_status, this]() {
        lbl_version_status->setText("Запрос...");

        api::CommandRequest request;
        request.set_command_id(8);

        api::CommandResponse response;
        grpc::ClientContext context;

        auto stub = ::api::MainService::NewStub(channel_);
        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            lbl_version_status->setText(QString::fromStdString(response.result_text()));
        } else {
            lbl_version_status->setText("Ошибка сети");
        }
    });

    content_widget->setLayout(main_layout);
    scroll_area->setWidget(content_widget);
    QVBoxLayout *outer_layout{create_v_box_layout(central_widget)};
    outer_layout->addWidget(scroll_area);
    central_widget->setLayout(outer_layout);
}

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

QLineEdit *QtViewPKU::create_line_edit(QWidget *const parent) const { return new (std::nothrow) QLineEdit{parent}; }
QComboBox *QtViewPKU::create_combo_box(QWidget *const parent) const { return new (std::nothrow) QComboBox{parent}; }
QTextEdit *QtViewPKU::create_text_edit(QWidget *const parent) const { return new (std::nothrow) QTextEdit{parent}; }
QPushButton *QtViewPKU::create_button(const QString &text, QWidget *const parent) const { return new (std::nothrow) QPushButton{text, parent}; }
QLabel *QtViewPKU::create_label(const QString &text) const { return new (std::nothrow) QLabel{text}; }
QWidget *QtViewPKU::create_widget() const { return new (std::nothrow) QWidget{}; }

QComboBox *QtViewPKU::create_multi_select_combo_box(QWidget *const parent, int count) const {
    QComboBox *combo = new (std::nothrow) QComboBox{parent};
    if (combo) {
        for (int i = 1; i <= count; ++i) { combo->addItem(QString("ПКУ %1").arg(i)); }
    }
    return combo;
}

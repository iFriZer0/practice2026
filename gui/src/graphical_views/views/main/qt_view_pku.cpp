#include <QComboBox>
#include <QTextEdit>
#include <iostream>
#include <fstream>
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
#include <grpcpp/grpcpp.h>
#include "pku_service.pb.h"
#include "pku_service.grpc.pb.h"


#include "qt_view_pku.h"

using namespace api;

std::string read_pku_service_address() {
    std::ifstream file("configuration/pku_service_address.txt");
    std::string address;
    if (file.is_open() && std::getline(file, address)) {
        address.erase(std::remove_if(address.begin(), address.end(), ::isspace), address.end());
        return address;
    }
    return "localhost:50051";
}

QtViewPKU::QtViewPKU(QStackedWidget *const stacked_widget) noexcept
    : stacked_widget{stacked_widget}
{
    std::string target_address = read_pku_service_address();
    auto channel = grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials());
    channel_ = grpc::CreateChannel(target_address, grpc::InsecureChannelCredentials());





    central_widget = create_widget();
    QVBoxLayout *main_layout = create_v_box_layout(central_widget);

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
    QLabel *rk_time_label = create_label("Длительность (мс)");
    QLineEdit *rk_time_input = create_line_edit(central_widget);
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
    QLabel *lbl_multi_pku = create_label("Выбор номеров ПКУ:");
    read_pku_layout->addWidget(lbl_multi_pku);

    QComboBox *multi_pku_select = create_multi_select_combo_box(central_widget, 64);
    multi_pku_select->setFixedWidth(250);
    read_pku_layout->addWidget(multi_pku_select);

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


    QObject::connect(btn_check_conn, &QPushButton::clicked, [=]() {
        lbl_connect_status->setText("Запрос...");

        ::api::CommandRequest request;
        request.set_command_id(1);

        ::api::CommandResponse response;
        grpc::ClientContext context;

        auto stub = ::api::MainService::NewStub(channel_);



        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            lbl_connect_status->setText(QString::fromStdString(response.result_text()));
        } else {
            lbl_connect_status->setText("Ошибка сети");
        }
    });

    QObject::connect(btn_get_status, &QPushButton::clicked, [=]() {
        lbl_equipment_status->setText("Запрос...");

        api::CommandRequest request;
        request.set_command_id(2);

        api::CommandResponse response;
        grpc::ClientContext context;

        auto stub = ::api::MainService::NewStub(channel_);
        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            lbl_equipment_status->setText(QString::fromStdString(response.result_text()));
        } else {
            lbl_equipment_status->setText("Ошибка сети");
        }
    });


    QObject::connect(btn_get_version, &QPushButton::clicked, [=]() {
        lbl_version_status->setText("Запрос...");

        api::CommandRequest request;
        request.set_command_id(3);

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



    QObject::connect(btn_read_main, &QPushButton::clicked, [=]() {
        lbl_read_main_status->setText("Чтение...");

        api::CommandRequest request;
        request.set_command_id(4);

        api::CommandResponse response;
        grpc::ClientContext context;

        auto stub = ::api::MainService::NewStub(channel_);
        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            QStringList parts = QString::fromStdString(response.result_text()).split(';');
            lbl_read_main_status->setText("Успешно прочитано");
            pku_log->append("--- Разделенная информация устройства ---");
            for (const QString& part : parts) {
                pku_log->append(part);
            }
        } else {
            lbl_read_main_status->setText("Ошибка чтения");
        }
    });



    QObject::connect(btn_write_main, &QPushButton::clicked, [=]() {
        lbl_write_main_status->setText("Запись...");

        api::CommandRequest request;
        request.set_command_id(5);

        api::CommandResponse response;
        grpc::ClientContext context;

        auto stub = ::api::MainService::NewStub(channel_);
        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            lbl_write_main_status->setText("Успешно записано");
        } else {
            lbl_write_main_status->setText("Ошибка записи");
        }
    });


    QObject::connect(btn_send_rk, &QPushButton::clicked, [=]() {
        QString param = rk_num_input->text() + ";" + rk_time_input->text();
        lbl_send_rk_status->setText("Отправка...");

        ::api::CommandRequest request;
        request.set_command_id(6);
        request.set_command_param(param.toStdString());

        ::api::CommandResponse response;
        grpc::ClientContext context;


        auto stub = ::api::MainService::NewStub(channel_);
        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            lbl_send_rk_status->setText("Выполнено: " + QString::fromStdString(response.result_text()));
        } else {
            lbl_send_rk_status->setText("Ошибка сети");
        }
    });


    QObject::connect(btn_set_mode, &QPushButton::clicked, [=]() {
        QString param = chan_select->currentText() + ";" + QString::number(mode_select->currentIndex());
        lbl_set_mode_status->setText("Применение...");

        api::CommandRequest request;
        request.set_command_id(7);
        request.set_command_param(param.toStdString());


        api::CommandResponse response;
        grpc::ClientContext context;

        auto stub = ::api::MainService::NewStub(channel_);

        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            lbl_set_mode_status->setText("Режим изменен: " + QString::fromStdString(response.result_text()));
        } else {
            lbl_set_mode_status->setText("Ошибка");
        }
    });

    QObject::connect(btn_read_pku, &QPushButton::clicked, [=]() {
        QString param = multi_pku_select->currentText();
        lbl_read_pku_status->setText("Чтение...");

        api::CommandRequest request;
        request.set_command_id(8);
        request.set_command_param(param.toStdString());


        api::CommandResponse response;
        grpc::ClientContext context;

        auto stub = ::api::MainService::NewStub(channel_);

        grpc::Status status = stub->SendCommand(&context, request, &response);

        if (status.ok()) {
            lbl_read_pku_status->setText("Успешно прочитано");

        } else {
            lbl_read_pku_status->setText("Ошибка чтения");
        }
    });


    central_widget->setLayout(main_layout);
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

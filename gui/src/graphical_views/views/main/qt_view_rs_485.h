#ifndef VIEW_RS_485_H__
#define VIEW_RS_485_H__

#include <memory>

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QStackedWidget>
#include <QString>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>

#include "view.h"

#include "rs485_service.h"

class QtViewRS485 : public View
{
public:
    explicit QtViewRS485(
        QStackedWidget *const stacked_widget
    );

    QtViewRS485(
        const QtViewRS485 &other
    ) = delete;

    QtViewRS485(
        QtViewRS485 &&other
    ) = delete;

    ~QtViewRS485() override;

    QtViewRS485 &operator=(
        const QtViewRS485 &other
    ) = delete;

    QtViewRS485 &operator=(
        QtViewRS485 &&other
    ) = delete;

    void show() override;

private:
    static constexpr int NOT_FOUND{-1};

    QWidget *central_widget{nullptr};
    QStackedWidget *stacked_widget{nullptr};

    std::shared_ptr<Rs485Service> rs485_service_;

    QLineEdit *driver_endpoint_input{nullptr};
    QPushButton *connect_button{nullptr};

    QSpinBox *channel_input{nullptr};
    QLineEdit *bytes_input{nullptr};

    QPushButton *send_button{nullptr};
    QPushButton *subscribe_button{nullptr};

    QTextEdit *sent_log{nullptr};
    QTextEdit *received_log{nullptr};

    QLabel *status_label{nullptr};

    bool subscribed{false};

    QWidget *create_widget() const;

    QVBoxLayout *create_v_box_layout(
        QWidget *parent
    ) const;

    QLabel *create_label(
        const QString &text
    ) const;

    void setup_ui();

    void set_driver_controls_enabled(
        bool enabled
    );

    void on_connect_clicked();

    void on_send_clicked();

    void on_subscribe_clicked();

    void handle_received_data(
        const ReceiveDataResult &result
    );

    void append_sent_log(
        const QString &text
    );

    void append_received_log(
        const QString &text
    );

    static QString packet_to_hex(
        const ReceiveDataPacket &packet
    );
};

#endif
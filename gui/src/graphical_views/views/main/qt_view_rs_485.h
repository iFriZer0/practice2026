#ifndef VIEW_RS_485_H__
#define VIEW_RS_485_H__

#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QComboBox>
#include <QSpinBox>

#include "view.h"

class QtViewRS485 : public View
{
public:
    explicit QtViewRS485(QStackedWidget *const stacked_widget) noexcept;

    QtViewRS485(const QtViewRS485 &other) = delete;
    QtViewRS485(QtViewRS485 &&other) = delete;

    ~QtViewRS485() = default;

    QtViewRS485 &operator=(const QtViewRS485 &other) = delete;
    QtViewRS485 &operator=(QtViewRS485 &&other) = delete;

    void show() override;

private:
    static constexpr int NOT_FOUND{-1};

    QWidget *central_widget;
    QStackedWidget *stacked_widget;

    QLineEdit *driver_endpoint_input;
    QSpinBox *channel_input;
    QLineEdit *bytes_input;

    QPushButton *send_button;
    QPushButton *subscribe_button;

    QTextEdit *sent_log;
    QTextEdit *received_log;

    QLabel *status_label;

    bool subscribed;

    QWidget *create_widget() const;
    QVBoxLayout *create_v_box_layout(QWidget *const parent) const;
    QLabel *create_label(const QString &text) const;

    void setup_ui();

    void on_send_clicked();
    void on_subscribe_clicked();

    void append_sent_log(const QString &text);
    void append_received_log(const QString &text);
};

#endif
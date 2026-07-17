#ifndef QT_VIEW_PKU_H__
#define QT_VIEW_PKU_H__

#include <QStackedWidget>
#include <QStandardItemModel>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include <QGroupBox>
#include <QGridLayout>
#include <QCheckBox>
#include <QScrollArea>
#include <QListWidget>
#include <QIntValidator>
#include <QObject>
#include <cstdint>
#include <memory>
#include <typeinfo>
#include "error.h"
#include "view.h"
namespace grpc {
class ChannelInterface;
}

class ConnectionError : public Error {
public:
    ConnectionError(const char *message, const std::type_info &first_error) noexcept;

    const void *get_data() const noexcept override;
};


class QtViewPKU : public View
{
public:
    explicit QtViewPKU(QStackedWidget *const stacked_widget);
    QtViewPKU(const QtViewPKU &other) = delete;
    QtViewPKU(QtViewPKU &&other) = delete;

    ~QtViewPKU() = default;

    QtViewPKU &operator=(const QtViewPKU &other) = delete;
    QtViewPKU &operator=(QtViewPKU &&other) = delete;

    void show() override;
private:
    static constexpr int NOT_FOUND{-1};
    QHBoxLayout *create_h_box_layout() const;
    QLineEdit *create_line_edit(QWidget *const parent) const;
    QComboBox *create_combo_box(QWidget *const parent) const;
    QTextEdit *create_text_edit(QWidget *const parent) const;
    QPushButton *create_button(const QString &text, QWidget *const parent) const;
    QComboBox *create_multi_select_combo_box(QWidget *const parent, int count) const;
    QWidget *create_widget() const;
    QVBoxLayout *create_v_box_layout(QWidget *const parent) const;
    QLabel *create_label(const QString &text) const;
    QGroupBox *create_group_box(const QString &title, QWidget *const parent = nullptr) const noexcept;
    QGridLayout *create_grid_layout(QWidget *const parent = nullptr) const noexcept;
    QCheckBox *create_check_box(const QString &text, QWidget *const parent = nullptr) const noexcept;
    QScrollArea *create_scroll_area(QWidget *const parent = nullptr) const noexcept;
    QListWidget *create_list_widget(QWidget *const parent = nullptr) const noexcept;
    QIntValidator *create_int_validator(const int minimum, const int maximum, QObject *const parent = nullptr) const noexcept;

    QString parse_mac(const QString &mac) const noexcept;
    QString parse_address(const QString &address) const noexcept;

    QWidget *central_widget;
    QStackedWidget *stacked_widget;

    std::shared_ptr<grpc::ChannelInterface> channel_;

    std::uint64_t operation_identifier{0};
};

#endif

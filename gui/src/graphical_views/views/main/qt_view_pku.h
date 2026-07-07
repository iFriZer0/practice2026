#ifndef QT_VIEW_PKU_H__

#define QT_VIEW_PKU_H__

#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QTextEdit>
#include <QPushButton>
#include "view.h"

class QtViewPKU : public View
{
public:
    explicit QtViewPKU(QStackedWidget *const stacked_widget) noexcept;
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

    QWidget *central_widget;
    QStackedWidget *stacked_widget;

    QWidget *create_widget() const;

    QVBoxLayout *create_v_box_layout(QWidget *const parent) const;

    QLabel *create_label(const QString &text) const;
};

#endif

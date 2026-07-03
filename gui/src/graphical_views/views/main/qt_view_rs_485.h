#ifndef VIEW_RS_485_H__

#define VIEW_RS_485_H__

#include <QStackedWidget>
#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QString>
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

    QWidget *create_widget() const;

    QVBoxLayout *create_v_box_layout(QWidget *const parent) const;

    QLabel *create_label(const QString &text) const;
};

#endif
